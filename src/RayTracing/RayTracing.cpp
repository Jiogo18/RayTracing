#include "RayTracing.h"

RayTracing::RayTracing(const map3D *map) : QThread()
{
    this->map = map;
    rtRess = new RayTracingRessources(map->getWorld(), map->getClient(), &dt);

    connect(map->getWorld(), &World::changed, this, &RayTracing::onWorldChanged);

    workerDistributor = new RayTracingDistributor(rtRess);
    RayTracingWorker *rayWorkers = workerDistributor->getWorkers();
    for (int i = 0; i < workerDistributor->nb_workers; i++) {
        connect(&rayWorkers[i], &RayTracingWorker::resultReady, this, &RayTracing::onRayWorkerReady);
    }
}

RayTracing::~RayTracing()
{
    delete workerDistributor;
    delete rtRess;
    if (lightPerColumn != nullptr) delete[] lightPerColumn;
}

void RayTracing::onRayWorkerReady(const int &x, const int &nbColumns, const PixScreen<ColorLight> &c, const int *totalLight)
{
    processFinished += nbColumns;
    for (int i = 0; i < nbColumns; i++) {
        lightPerColumn[x + i] = totalLight[i];
        colors.setColumn(x + i, c.getColumn(i));
    }

    if (processFinished >= processWidth) {
        onAllWorkersFinished();
    }
#ifdef REFRESH_COLUMN
    else {
        if (processForUpdate <= processFinished) {
            paint();
            processForUpdate = processFinished + RAYTRACING::RefreshColumn;
        }
    }
#endif // REFRESH_COLUMN
}

void RayTracing::onWorldChanged(const WorldChange &change)
{
    switch (change.type()) {
    case WorldChange::Type::block:
    case WorldChange::Type::chunk: {
        rtRess->onWorldChanged();
        break;
    }
    case WorldChange::Type::entity:
        break;
    }
}

void RayTracing::run()
{
    dt.clear();
    startRun = dt.getCurrent();

    processWidth = calcSize.width();
    processFinished = 0;
    processForUpdate = RAYTRACING::RefreshColumn;

    if (calcSize != colors.size()) {
        // il ne faut pas en attribuer trop sinon trop de colonnes pour un thread
        // 1920 =>36 colonnes par worker
        // 150 => 10 colonnes par worker
        int columnsPerWorker = 20 * std::sqrt(calcSize.width()) / RAYTRACING::WorkerThread;
        qDebug() << "ColumnsPerWorker" << columnsPerWorker << calcSize << WorkerThread;

        for (int i = 0; i < workerDistributor->nb_workers; i++) {
            workerDistributor->getWorkers()[i].setPrimaryWork(calcSize, columnsPerWorker);
        }
        colors = PixScreen<ColorLight>(calcSize);
        if (lightPerColumn != nullptr) delete[] lightPerColumn;
        lightPerColumn = new int[processWidth];
        for (int i = 0; i < processWidth; i++)
            lightPerColumn[i] = 0;
#ifdef REFRESH_COLUMN
        image = image.scaled(calcSize);
#else
        image = RayImage(calcSize);
#endif // REFRESH_COLUMN
    }

#ifdef REFRESH_COLUMN
    for (int x = 0; x < image.width(); x++) {
        image.setPixelColor(x, 0, Qt::white);
        image.setPixelColor(x, 1, Qt::white);
    }
#endif // REFRESH_COLUMN

    rtRess->resetRessources(map->getClient()); // reset the pos

#ifdef DISABLE_WORKER_DISTRIBUTOR
    onAllWorkersFinished();
#else
    workerDistributor->start(processWidth);
#endif // DISABLE_WORKER_DISTRIBUTOR

    // N.B. this function takes less than 1 msec
}

void RayTracing::paint()
{
    qint64 start = dt.getCurrent();
    int x, y;
    const double totalLight = calcTotalLight(); // < 1 ms

    const QList<ColorLight> *column;

    // 10 ms en full screen
    for (x = 0; x < colors.width(); x++) {
        column = &colors.getColumn(x);
        for (y = 0; y < colors.height(); y++) {
            image.setPixel(x, y, column->at(y).getColorReduced(totalLight));
        }
    }

    dt.addValue("paint", dt.getCurrent() - start);
    emit resultReady();
}

void RayTracing::onAllWorkersFinished()
{
    qDebug() << "All ray workers have finished, processFinished :" << processFinished << "/" << processWidth;
    workerDistributor->stop();
    paint();
    dt.addValue("run", dt.getCurrent() - startRun);
    qDebug() << "GUIWorker::run #end" << dt;
}
