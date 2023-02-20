#pragma once

#include <vector>
#include "../World/map3D.h"
#include "../utils/ComputeShader.h"

namespace GPUCalls {

    std::vector<int> hasIntersection(const map3D *map, SIZE screnSize);

    void fillImage(SIZE screenSize, uchar *image);

    void calcImage(const map3D *map, SIZE screenSize, uchar *image);

    void init();
    void resetQueue();
    void changeKernelFillImage(const std::string &functionName);

}; // namespace GPUCalls