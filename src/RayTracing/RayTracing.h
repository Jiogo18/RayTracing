#ifndef RAYTRACING_H
#define RAYTRACING_H

#include "Worker.h"
#include "RayImage.h"

class RayTracing
{
public:
    RayTracing(const map3D *map);
    ~RayTracing();

    RayTracing *setSize(const QSize &size)
    {
        if (!isRunning()) calcSize = size;
        return this;
    }
    const RayImage *getImage() const { return &image; }
    bool isRunning() { return running || workerDistributor->isRunning(); }

    void start() {}
    void quit() { quit_asked = true; }
    void wait(int timeout) { thread.join(); }

    // private slots:

    // signals:
    void resultReady();

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
    QSize calcSize;

    PixScreen<ColorLight> colors;
    int *lightPerColumn = nullptr; // calculs des light de chaque colonne (en fonction de x)

    RayTracingDistributor *workerDistributor;
    int processWidth = 0;
    int processFinished = 0;
    int processForUpdate;

    RayImage image;

    std::thread thread;
    bool running = false;
    bool quit_asked = false;
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
