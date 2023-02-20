#include "GPUCalls.h"

#define BytesPerPixel 4
#define CL_PERM_READ CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR
#define CL_PERM_WRITE CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR
#define CL_PERM_ALL CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR

cl::Kernel kernelFillImage;

namespace GPUCalls {

    std::vector<int> hasIntersection(const map3D *map, SIZE screenSize)
    {
        const unsigned int pixelCount = screenSize.cx * screenSize.cy;

        Size3D chunkSize = Chunk::getSize();
        std::vector<Point3D> chunkPos;
        const std::vector<Chunk *> &chunks = map->getWorld()->getChunks();
        int chunkCount = chunks.size();
        chunkPos.resize(chunks.size());
        for (unsigned int i = 0; i < chunks.size(); i++) {
            chunkPos[i] = chunks[i]->getMaxGeometry().getPointMin();
        }

        std::vector<int> solidsCountPerChunk;
        std::vector<std::vector<Point3D>> solidsPointMin;
        std::vector<std::vector<Point3D>> solidsPointMax;
        solidsCountPerChunk.resize(chunks.size());
        solidsPointMin.resize(chunks.size());
        solidsPointMax.resize(chunks.size());
        for (unsigned int i = 0; i < chunks.size(); i++) {
            const std::vector<Solid *> solids = *chunks[i]->getSolids();
            solidsCountPerChunk[i] = solids.size();
            solidsPointMin[i].resize(solids.size());
            solidsPointMax[i].resize(solids.size());
            for (unsigned int j = 0; j < solids.size(); j++) {
                solidsPointMin[i][j] = solids[j]->getMaxGeometry().getPointMin();
                solidsPointMax[i][j] = solids[j]->getMaxGeometry().getPointMax();
            }
        }

        // cl::Buffer camera_pos_buffer = ComputeShader::Buffer(map->getClient()->getPoint(), Permissions::Read);    // double[3]
        // cl::Buffer camera_rot_buffer = ComputeShader::Buffer(map->getClient()->getRot(), Permissions::Read);      // double[2]
        // cl::Buffer chunk_count_buffer = ComputeShader::Buffer(chunkCount, Permissions::Read);                     // int
        // cl::Buffer chunk_pos_buffer = ComputeShader::Buffer(chunkPos, Permissions::Read);                         // double[chunkCount][3]
        // cl::Buffer solids_count_per_chunk_buffer = ComputeShader::Buffer(solidsCountPerChunk, Permissions::Read); // int[chunkCount]
        // cl::Buffer solids_min_buffer = ComputeShader::Buffer(solidsPointMin, Permissions::Read);                  // double[chunkCount][solidsCountPerChunk][3]
        // cl::Buffer solids_max_buffer = ComputeShader::Buffer(solidsPointMax, Permissions::Read);                  // double[chunkCount][solidsCountPerChunk][3]

        std::vector<int> results;
        results.resize(pixelCount);
        cl::Buffer result_buffer = ComputeShader::Buffer(results, Permissions::All); // bool[pixelCount]

        // ComputeShader::launch("has_intersection", {&result_buffer, &camera_pos_buffer, &camera_rot_buffer, &chunk_count_buffer, &chunk_pos_buffer, &solids_count_per_chunk_buffer, &solids_min_buffer, &solids_max_buffer}, pixelCount);
        ComputeShader::launch("test", {&result_buffer}, cl::NDRange(screenSize.cx, screenSize.cy));
        ComputeShader::get_data(result_buffer, results);

        return results;
    }

    /**
     * Fills the image using the function defined in kernelFillImage.
     * The function must have the following signature:
     * __kernel void function_name(__global uchar *imageRGBA)
     * The coordinates of the pixel are given by the global id (get_global_id(0) and get_global_id(1)).
     */
    void fillImage(SIZE screenSize, uchar *image)
    {
        const unsigned int pixelCount = screenSize.cx * screenSize.cy;
        const unsigned int size = sizeof(uchar) * BytesPerPixel * pixelCount;

        cl::Buffer image_buffer(ComputeShader::context, CL_PERM_WRITE, size, image);
        kernelFillImage.setArg(0, image_buffer);

        ComputeShader::queue.enqueueNDRangeKernel(kernelFillImage, cl::NullRange, cl::NDRange(screenSize.cx, screenSize.cy));

        ComputeShader::queue.enqueueReadBuffer(image_buffer, CL_TRUE, 0, size, image);
    }

    void calcImage(const map3D *map, SIZE screenSize, uchar *image)
    {
        // image as {R, G, B, A} * pixelCount
        // r as {255*255*255} * pixelCount

        fillImage(screenSize, image);
    }

    /**
     * Initializes the OpenCL context and the queue.
     */
    void init()
    {
        ComputeShader::init("shaders/compute_shader.cl");
        changeKernelFillImage("color_wheel");
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

}; // namespace GPUCalls