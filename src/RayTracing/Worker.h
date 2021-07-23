#ifndef WORKER_H
#define WORKER_H

#include <QThread>
#include "Ressources.h"
#include "Ray.h"

class RayTracingWorker : public QThread
{
    Q_OBJECT
public:
    RayTracingWorker();
    RayTracingWorker(const int &workerId, RayTracingRessources *rtRess, QObject *parent = nullptr);
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

signals:
    void resultReady(int x, int nbColumns, const PixScreen<ColorLight> &c, const int *totalLight);

private:
    void run() override; // process ONE column at a time (more with nbColumn)
    int workerId;
    QSize sceneSize;

    int xScene; // just ONE column, more columns with nbColumn
    int nbColumn;
    RayTracingRessources *rtRess;
    int *totalLight = nullptr;
    PixScreen<ColorLight> colors;
};

class RayTracingDistributor : public QThread
{
    Q_OBJECT
public:
    RayTracingDistributor(RayTracingRessources *rtRess);
    ~RayTracingDistributor();

    void start(const int &processWidth);
    void stop();

    RayTracingWorker *getWorkers() const { return workers; }
    const int nb_workers = RAYTRACING::WorkerThread;
    inline bool isRunning() const { return QThread::isRunning() || workersInProcess; }

private slots:
    void onRayWorkerFinished();

private:
    void assignNextRayWork(RayTracingWorker *worker);
    RayTracingWorker *workers = nullptr;
    int processWidth = 0;
    int processStarted = 0;
    bool workersInProcess = false;
    qint64 timeStart;
};

#endif // WORKER_H
