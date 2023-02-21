#include "GPUCalls.h"

#define BytesPerPixel 4
#define CL_PERM_READ CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR
#define CL_PERM_WRITE CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR
#define CL_PERM_ALL CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR

cl::Kernel kernelFillImage;
cl::Kernel kernelRayTracing;
int gpuMode = 1;

typedef struct
{
    double x;
    double y;
    double z;
} Point3D_t;

typedef struct
{
    double x;
    double y;
} Rot3D_t;

typedef struct
{
    unsigned long int solidsCount;
    unsigned long int solidsOffset;
} ChunkData_t;

Point3D_t Point3DToStruct(const Point3D &point)
{
    return {(double)point.x(), (double)point.y(), (double)point.z()};
}

Rot3D_t Rot3DToStruct(const Rot3D &rot)
{
    return {rot.rX(), rot.rZ()};
}

namespace GPUCalls {
    /**
     * Fills the image using the function defined in the kernel.
     * The function must have the following signature:
     * __kernel void function_name(__global uchar *imageRGBA, ...)
     * The coordinates of the pixel are given by the global id (get_global_id(0) and get_global_id(1)).
     * Don't use the first argument of the kernel, it is used to pass the image buffer.
     */
    void computeShader(SIZE screenSize, uchar *image, cl::Kernel &kernel)
    {
        const unsigned int pixelCount = screenSize.cx * screenSize.cy;
        const unsigned int size = sizeof(uchar) * BytesPerPixel * pixelCount;

        cl::Buffer image_buffer(ComputeShader::context, CL_PERM_WRITE, size, image);
        kernel.setArg(0, image_buffer);

        ComputeShader::queue.enqueueNDRangeKernel(kernel, cl::NullRange, cl::NDRange(screenSize.cx, screenSize.cy));

        ComputeShader::queue.enqueueReadBuffer(image_buffer, CL_TRUE, 0, size, image);
    }

    void fillImage(SIZE screenSize, uchar *image)
    {
        computeShader(screenSize, image, kernelFillImage);
    }

    void fillRayTracing(SIZE screenSize, uchar *image, const map3D *map)
    {
        Point3D_t camera_pos = Point3DToStruct(map->getClient()->getPoint());
        Rot3D_t camera_rot = Rot3DToStruct(map->getClient()->getRot());

        std::vector<Point3D_t> chunkPos;
        const std::vector<Chunk *> &chunks = map->getWorld()->getChunks();
        int chunkCount = chunks.size();
        chunkPos.resize(chunks.size());
        for (unsigned int i = 0; i < chunks.size(); i++) {
            chunkPos[i] = Point3DToStruct(chunks[i]->getMaxGeometry().getPointMin());
        }

        std::vector<ChunkData_t> chunkData;
        std::vector<Point3D_t> solidsPointMin;
        std::vector<Point3D_t> solidsPointMax;
        chunkData.resize(chunks.size());
        for (unsigned int i = 0; i < chunks.size(); i++) {
            const std::vector<Solid *> solids = *chunks[i]->getSolids();
            chunkData[i] = {(unsigned long int)solids.size(), (unsigned long int)solidsPointMin.size()};
            for (const Solid *solid : solids) {
                solidsPointMin.push_back(Point3DToStruct(solid->getMaxGeometry().getPointMin()));
                solidsPointMax.push_back(Point3DToStruct(solid->getMaxGeometry().getPointMax()));
            }
        }

        cl::Buffer camera_pos_buffer(ComputeShader::context, CL_PERM_READ, sizeof(Point3D_t), &camera_pos);                                   // double[3]
        cl::Buffer camera_rot_buffer(ComputeShader::context, CL_PERM_READ, sizeof(Rot3D_t), &camera_rot);                                     // double[2]
        cl::Buffer chunk_count_buffer(ComputeShader::context, CL_PERM_READ, sizeof(int), &chunkCount);                                        // int
        cl::Buffer chunk_pos_buffer(ComputeShader::context, CL_PERM_READ, sizeof(Point3D_t) * chunkCount, chunkPos.data());                   // double[chunkCount][3]
        cl::Buffer chunk_data_buffer(ComputeShader::context, CL_PERM_READ, sizeof(ChunkData_t) * chunkCount, chunkData.data());               // ulong[chunkCount][2]
        cl::Buffer solids_min_buffer(ComputeShader::context, CL_PERM_READ, sizeof(Point3D_t) * solidsPointMin.size(), solidsPointMin.data()); // double[chunkCount][solidsCount][3]
        cl::Buffer solids_max_buffer(ComputeShader::context, CL_PERM_READ, sizeof(Point3D_t) * solidsPointMax.size(), solidsPointMax.data()); // double[chunkCount][solidsCount][3]

        // Argument 0 set to the image buffer (start at 1)
        kernelRayTracing.setArg(1, camera_pos_buffer);
        kernelRayTracing.setArg(2, camera_rot_buffer);
        kernelRayTracing.setArg(3, chunk_count_buffer);
        kernelRayTracing.setArg(4, chunk_pos_buffer);
        kernelRayTracing.setArg(5, chunk_data_buffer);
        kernelRayTracing.setArg(6, solids_min_buffer);
        kernelRayTracing.setArg(7, solids_max_buffer);

        computeShader(screenSize, image, kernelRayTracing);
    }

    void calcImage(const map3D *map, SIZE screenSize, uchar *image)
    {
        // image as {R, G, B, A} * pixelCount
        // r as {255*255*255} * pixelCount

        switch (gpuMode) {
        case 0:
            fillImage(screenSize, image);
            break;
        case 1:
            fillRayTracing(screenSize, image, map);
            break;
        }
    }

    /**
     * Initializes the OpenCL context and the queue.
     */
    void init()
    {
        ComputeShader::init("shaders/compute_shader.cl");
        changeKernelFillImage("color_wheel");
        changeKernelRayTracing("ray_tracing");
        resetQueue();
    }

    /**
     * Should be called when the OpenCL context is lost.
     * This will recreate the context and the queue.
     * Can be called just before using it, but it will be slower.
     */
    void resetQueue()
    {
        ComputeShader::queue = cl::CommandQueue(ComputeShader::context, ComputeShader::device);
    }

    /**
     * Create a new kernel with the given name.
     * The kernel will be used by the fillImage function.
     */
    void changeKernelFillImage(const std::string &functionName)
    {
        kernelFillImage = cl::Kernel(ComputeShader::program, functionName.c_str());
    }

    void changeKernelRayTracing(const std::string &functionName)
    {
        kernelRayTracing = cl::Kernel(ComputeShader::program, functionName.c_str());
    }

    /**
     * Reload the OpenCL program and recreate the kernels.
     * Switch between the kernels to render the image.
     */
    void switchGPUMode()
    {
        gpuMode++;
        if (gpuMode > 1) {
            gpuMode = 0;
        }
        switch (gpuMode) {
        case 0:
            changeKernelFillImage("color_wheel");
            break;
        case 1:
            changeKernelRayTracing("ray_tracing");
            break;
        }
        resetQueue();
    }

}; // namespace GPUCalls