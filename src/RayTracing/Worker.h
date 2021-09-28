#ifndef WORKER_H
#define WORKER_H

#include <thread>
#include "Ressources.h"
#include "PixScreen.h"
#include "Ray.h"

class RayTracingWorker
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

    // signals:
    void resultReady(int x, int nbColumns, const PixScreen<ColorLight> &c, const int *totalLight);

    // TODO
    void start() { running = false; }
    bool isRunning() const { return running; }
    void quit() { quit_asked = true; }
    void wait(int time)
    {
        this->thread->join();
        running = false;
    }

private:
    void run(); // process ONE column at a time (more with nbColumn)
    int workerId;
    QSize sceneSize;

    int xScene; // just ONE column, more columns with nbColumn
    int nbColumn;
    RayTracingRessources *rtRess;
    int *totalLight = nullptr;
    PixScreen<ColorLight> colors;

    std::thread *thread = nullptr;
    bool running = false;
    bool quit_asked = false;
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

    // private slots:
    void onRayWorkerFinished();

private:
    void assignNextRayWork(RayTracingWorker *worker);
    RayTracingWorker *workers = nullptr;
    int processWidth = 0;
    int processStarted = 0;
    bool workersInProcess = false;
    int64_t timeStart;

    std::thread *thread;
};

#endif // WORKER_H
