#include "Worker.h"

RayTracingWorker::RayTracingWorker() : Thread(), workerId(-1), rtRess(nullptr), totalLight(nullptr) {}

RayTracingWorker::RayTracingWorker(const int &workerId, RayTracingRessources *rtRess) : Thread(),
                                                                                        workerId(workerId),
                                                                                        rtRess(rtRess),
                                                                                        totalLight(nullptr)
{}

RayTracingWorker::~RayTracingWorker()
{
    if (totalLight != nullptr) delete[] totalLight;
}

RayTracingWorker *RayTracingWorker::operator=(const RayTracingWorker &worker)
{
    workerId = worker.workerId;
    rtRess = worker.rtRess;
    return this;
}

RayTracingWorker *RayTracingWorker::setPrimaryWork(const SIZE &sceneSize, const int &nbColumn)
{
    this->sceneSize = sceneSize;
    colors = PixScreen<ColorLight>(nbColumn, sceneSize.cy); // inversement des lignes et des colonnes
    if (this->nbColumn != nbColumn) {
        this->nbColumn = nbColumn;
        if (totalLight != nullptr) delete[] totalLight;
        totalLight = new int[nbColumn];
    }
    return this;
}

void RayTracingWorker::connectResultReady(std::function<void(int, int, const PixScreen<ColorLight> &, const int *)> callback)
{
    resultReadyCallback = callback;
}

void RayTracingWorker::run()
{
    int i;
    for (i = 0; i < getNbColumn(); i++) {
        int64_t start;
        // start = rtRess->dt->getCurrent();
        totalLight[i] = 0;
        // pos en % de pixmap.width/2 * xMax
        doubli xPos = (2.0L * (xScene + i) / sceneSize.cx - 1) * xMax, xzPos = sqrt(xPos * xPos + 1);
        radian angleH = atanSimple(xPos);
        // rtRess->dt->addValue("RayTracingWorker::run_init", rtRess->dt->getCurrent() - start);//1ms

        start = rtRess->dt->getCurrent();
        for (int y = 0; y < sceneSize.cy; y++) {
            // qint64 start2;

            // start2 = rtRess->dt->getCurrent();
            // pos en % de pixmap.height/2 * yMax
            doubli yPos = (-1 + 2.0L * y / sceneSize.cy) * yMax;
            Ray ray(rtRess->clientPos.getChildRot(angleH, atanSimple(yPos / xzPos)), rtRess);
            // rtRess->dt->addValue("RayTracingWorker::run_1_Ray::Ray", rtRess->dt->getCurrent() - start2);//1080ms

#ifndef DISABLE_RAYPROCESS
            // start2 = rtRess->dt->getCurrent();
            ray.process();
            // rtRess->dt->addValue("RayTracingWorker::run_2_process", rtRess->dt->getCurrent() - start2);
#endif // DISABLE_RAYPROCESS

            // start2 = rtRess->dt->getCurrent();
            ColorLight colorL = ray.getColor(RAYTRACING::gamma);

            totalLight[i] += colorL.getLight();
            colors.set(i, y, colorL);
            // rtRess->dt->addValue("RayTracingWorker::run_3_ColorLight", rtRess->dt->getCurrent() - start2);//150ms
        }
        rtRess->dt->addValue("RayTracingWorker::run_colors", rtRess->dt->getCurrent() - start);
    }

    resultReadyCallback(xScene, i, colors, totalLight);
}

RayTracingDistributor::RayTracingDistributor(RayTracingRessources *rtRess)
{
    workers = new RayTracingWorker[nb_workers];
    for (int i = 0; i < nb_workers; i++) {
        workers[i] = RayTracingWorker(i, rtRess);
        workers[i].connectOnFinished([this, i]() { onRayWorkerFinished(i); });
    }
}

RayTracingDistributor::~RayTracingDistributor()
{
    for (int i = 0; i < nb_workers; i++)
        workers[i].quit();
    for (int i = 0; i < nb_workers; i++) {
        workers[i].wait(1000);
    }
    delete[] workers;
}

void RayTracingDistributor::start(const int &processWidth)
{
    timeStart = DebugTime::getCurrent();
    this->processWidth = processWidth;
    processStarted = 0;
    workersInProcess = true;
    mutexWorkFinished.lock();
    for (int i = 0; i < nb_workers && i < processWidth; i++) {
        assignNextRayWork(&workers[i]);
    }
    mutexWorkFinished.unlock();
    // then wait for the last onRayWorkerReady
}

void RayTracingDistributor::stop()
{
    processWidth = 0;
    processStarted = 0;
    workersInProcess = false;
    std::cout << "RayTracingDistributor stopped after " << DebugTime::getCurrent() - timeStart << " us" << std::endl;
}

void RayTracingDistributor::onRayWorkerFinished(int workerId)
{
    mutexWorkFinished.lock();
    assignNextRayWork(&workers[workerId]);
    mutexWorkFinished.unlock();
}

void RayTracingDistributor::assignNextRayWork(RayTracingWorker *worker)
{
    if (worker->isRunning()) {
        std::cout << "RayTracingWorker " << worker->getWorkerId() << " is already running" << std::endl;
        worker->wait(1);
        if (worker->isRunning()) {
            std::cout << "RayTracingWorker " << worker->getWorkerId() << " skipped" << std::endl;
            return;
        }
    }
    if (processStarted >= processWidth) return;
    worker->setWork(processStarted)->start();
    processStarted += worker->getNbColumn();
}
