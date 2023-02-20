#ifndef RAYTRACING_H
#define RAYTRACING_H

#include "Worker.h"
#include "../utils/Timer.h"

class RayTracing : public Thread
{
public:
    RayTracing(const map3D *map);
    ~RayTracing();

    RayTracing *setSize(SIZE size)
    {
        if (!isRunning()) calcSize = size;
        return this;
    }

    const Image *getImage() const { return &image; }

    void connectResultReady(std::function<void()> callback);

protected:
    const map3D *map;
    RayTracingRessources *rtRess = nullptr;
    DebugTime dt;
    int64_t startRun;

    SIZE calcSize;
    PixScreen<ColorLight> colorLightImage;
    Image image;

    // Calculer colorLightImage
    void run() override;
    virtual void onSizeChanged() = 0;
    virtual void startGeneration() = 0;

    // Calculer image à partir de colorLightImage
    virtual void updateImage();
    virtual constexpr double calcTotalLight() const = 0;

    // Callback, Image is ready
    void onImageReady();
    void onGenerationFinished();

private:
    std::function<void()> resultReadyCallback;
    void onWorldChanged(const WorldChange &change);
};

class RayTracingCPU : public RayTracing
{
public:
    RayTracingCPU(const map3D *map);
    ~RayTracingCPU();

protected:
    void onSizeChanged() override;
    void startGeneration() override;
    constexpr double calcTotalLight() const override;

private:
    void onRayWorkerReady(const int &x, const int &nbColumns, const PixScreen<ColorLight> &c, const int *totalLight);
    void onAllWorkersFinished();

    int *lightPerColumn = nullptr; // calculs des light de chaque colonne (en fonction de x)
    std::mutex mutexWorkerFinished;

    RayTracingDistributor *workerDistributor;
    int processWidth = 0;
    int processFinished = 0;
    int processForUpdate;
};

class RayTracingGPU : public RayTracing
{
public:
    RayTracingGPU(const map3D *map);
    ~RayTracingGPU();

protected:
    void onSizeChanged() override;
    void startGeneration() override;
    constexpr double calcTotalLight() const override;

private:
    Timer timerFinished;
};

#endif // RAYTRACING_H
