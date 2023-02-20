#pragma once

#include <vector>
#include "../World/map3D.h"
#include "../utils/ComputeShader.h"

namespace GPUCalls {

    void computeShader(SIZE screenSize, uchar *image, cl::Kernel &kernel);

    void fillImage(SIZE screenSize, uchar *image);
    void fillRayTracing(SIZE screenSize, uchar *image, const map3D *map);

    void calcImage(const map3D *map, SIZE screenSize, uchar *image);

    void init();
    void resetQueue();
    void changeKernelFillImage(const std::string &functionName);
    void changeKernelRayTracing(const std::string &functionName);

}; // namespace GPUCalls