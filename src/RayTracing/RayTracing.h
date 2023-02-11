#ifndef RAYTRACING_H
#define RAYTRACING_H

#include "Worker.h"

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
    bool isRunning() { return Thread::isRunning() || workerDistributor->isRunning(); }
    // private slots:

    void connectResultReady(std::function<void()> callback);

private:
    void onRayWorkerReady(const int &x, const int &nbColumns, const PixScreen<ColorLight> &c, const int *totalLight);
    void onWorldChanged(const WorldChange &change);

    void run();
    constexpr inline double calcTotalLight() const;
    void paint();
    void onAllWorkersFinished();

    RayTracingRessources *rtRess = nullptr;
    DebugTime dt;
    int64_t startRun;

    const map3D *map;
    SIZE calcSize;

    PixScreen<ColorLight> colors;
    int *lightPerColumn = nullptr; // calculs des light de chaque colonne (en fonction de x)

    RayTracingDistributor *workerDistributor;
    int processWidth = 0;
    int processFinished = 0;
    int processForUpdate;

    Image image;

    std::function<void()> resultReadyCallback;
};

constexpr inline double RayTracing::calcTotalLight() const
{
    double totalLight = 0; // une sorte de moyenne pour privilégier le poids des très lumineux
    for (int i = 0; i < processWidth; i++)
        totalLight += lightPerColumn[i];
    return totalLight / colors.rowNumber(); // moyenne par pixel
    // full screen : 2 msec / 20 appels
}

#endif // RAYTRACING_H
