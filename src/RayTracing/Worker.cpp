#include "Worker.h"

RayTracingWorker::RayTracingWorker() : QThread(), workerId(-1), rtRess(nullptr), totalLight(nullptr) {}

RayTracingWorker::RayTracingWorker(const int &workerId, RayTracingRessources *rtRess, QObject *parent)
    : QThread(parent),
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
    setParent(worker.parent());
    return this;
}

RayTracingWorker *RayTracingWorker::setPrimaryWork(const QSize &sceneSize, const int &nbColumn)
{
    this->sceneSize = sceneSize;
    colors = PixScreen<ColorLight>(nbColumn, sceneSize.height()); //inversement des lignes et des colonnes
    if (this->nbColumn != nbColumn) {
        this->nbColumn = nbColumn;
        if (totalLight != nullptr) delete[] totalLight;
        totalLight = new int[nbColumn];
    }
    return this;
}

void RayTracingWorker::run()
{
    int i;
    for (i = 0; i < getNbColumn(); i++) {
        qint64 start;
        //start = rtRess->dt->getCurrent();
        totalLight[i] = 0;
        //pos en % de pixmap.width/2 * xMax
        doubli xPos = (2.0L * (xScene + i) / sceneSize.width() - 1) * xMax, xzPos = sqrt(xPos * xPos + 1);
        radian angleH = atan(xPos);
        //rtRess->dt->addValue("RayTracingWorker::run_init", rtRess->dt->getCurrent() - start);//1ms

        start = rtRess->dt->getCurrent();
        for (int y = 0; y < sceneSize.height(); y++) {
            //qint64 start2;

            //start2 = rtRess->dt->getCurrent();
            //pos en % de pixmap.height/2 * yMax
            doubli yPos = (1 - 2.0L * y / sceneSize.height()) * yMax;
            Ray ray(rtRess->clientPos.getChildRot(angleH, atan(yPos / xzPos)), rtRess);
            //rtRess->dt->addValue("RayTracingWorker::run_1_Ray::Ray", rtRess->dt->getCurrent() - start2);//1080ms

#ifndef DISABLE_RAYPROCESS
            //start2 = rtRess->dt->getCurrent();
            ray.process(rtRess->world);
            //rtRess->dt->addValue("RayTracingWorker::run_2_process", rtRess->dt->getCurrent() - start2);
#endif // DISABLE_RAYPROCESS

            //start2 = rtRess->dt->getCurrent();
            ColorLight colorL = ray.getColor(RAYTRACING::gamma);

            totalLight[i] += colorL.getLight();
            colors.set(i, y, colorL);
            //rtRess->dt->addValue("RayTracingWorker::run_3_ColorLight", rtRess->dt->getCurrent() - start2);//150ms
        }
        rtRess->dt->addValue("RayTracingWorker::run_colors", rtRess->dt->getCurrent() - start);
    }

    emit resultReady(xScene, i, colors, totalLight);
}

RayTracingDistributor::RayTracingDistributor(RayTracingRessources *rtRess)
{
    workers = new RayTracingWorker[nb_workers];
    for (int i = 0; i < nb_workers; i++) {
        workers[i] = RayTracingWorker(i, rtRess);
        connect(&workers[i], &QThread::finished, this, &RayTracingDistributor::onRayWorkerFinished);
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
    this->processWidth = processWidth;
    processStarted = 0;
    workersInProcess = true;
    for (int i = 0; i < nb_workers && i < processWidth; i++) {
        assignNextRayWork(&workers[i]);
    }
    // then wait for the last onRayWorkerReady
}

void RayTracingDistributor::stop()
{
    processWidth = 0;
    processStarted = 0;
    workersInProcess = false;
}

void RayTracingDistributor::onRayWorkerFinished()
{
    assignNextRayWork(qobject_cast<RayTracingWorker *>(sender()));
}

void RayTracingDistributor::assignNextRayWork(RayTracingWorker *worker)
{
    if (worker->isRunning()) {
        qWarning() << "RayTracingWorker" << worker->getWorkerId() << "is already running";
        worker->wait(1);
        if (worker->isRunning()) {
            qWarning() << "RayTracingWorker" << worker->getWorkerId() << "skipped";
            return;
        }
    }
    if (processStarted >= processWidth) return;
    worker->setWork(processStarted)->start();
    processStarted += worker->getNbColumn();
}
