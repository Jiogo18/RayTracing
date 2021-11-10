#ifndef WORKER_H
#define WORKER_H

#include "../Qt_compat/Thread.h"
#include "Ressources.h"
#include "PixScreen.h"
#include "Ray.h"

class RayTracingWorker : public Thread
{
public:
    RayTracingWorker();
    RayTracingWorker(const int &workerId, RayTracingRessources *rtRess);
    ~RayTracingWorker();
    RayTracingWorker *operator=(const RayTracingWorker &worker);

    RayTracingWorker *setPrimaryWork(const QSize &sceneSize, const int &nbColumn);
    inline RayTracingWorker *setWork(int xScene)
    {
        this->xScene = xScene;
        return this;
    }
    constexpr inline const int &getWorkerId() const { return workerId; };
    constexpr inline int getNbColumn() const { return min(nbColumn, sceneSize.width() - xScene); }

    void connectResultReady(std::function<void(int, int, const PixScreen<ColorLight> &, const int *)> callback);

private:
    void run(); // process ONE column at a time (more with nbColumn)
    int workerId;
    QSize sceneSize;

    int xScene; // just ONE column, more columns with nbColumn
    int nbColumn;
    RayTracingRessources *rtRess;
    int *totalLight = nullptr;
    PixScreen<ColorLight> colors;

    std::function<void(int, int, const PixScreen<ColorLight> &, const int *)> resultReadyCallback; // x, nbColumns, c, tootalLight
};

class RayTracingDistributor
{
public:
    RayTracingDistributor(RayTracingRessources *rtRess);
    ~RayTracingDistributor();

    void start(const int &processWidth);
    void stop();

    RayTracingWorker *getWorkers() const { return workers; }
    const int nb_workers = RAYTRACING::WorkerThread;
    inline bool isRunning() const { return workersInProcess; }

private:
    void onRayWorkerFinished(int workerId);
    void assignNextRayWork(RayTracingWorker *worker);
    RayTracingWorker *workers = nullptr;
    int processWidth = 0;
    int processStarted = 0;
    bool workersInProcess = false;
    int64_t timeStart;
    bool startingDistribution = true;

    std::thread *thread;
};

#endif // WORKER_H
