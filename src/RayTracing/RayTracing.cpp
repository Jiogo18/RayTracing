#include "RayTracing.h"

RayTracing::RayTracing(const map3D *map) : Thread(), map(map)
{
    rtRess = new RayTracingRessources(map->getWorld(), map->getClient(), &dt);

    // connect(map->getWorld(), &World::changed, this, &RayTracing::onWorldChanged);

    workerDistributor = new RayTracingDistributor(rtRess);
    RayTracingWorker *rayWorkers = workerDistributor->getWorkers();
    for (int i = 0; i < workerDistributor->nb_workers; i++) {
        rayWorkers[i].connectResultReady([this](int x, int nbColumns, const PixScreen<ColorLight> &c, const int *totalLight) { this->onRayWorkerReady(x, nbColumns, c, totalLight); });
    }
}

RayTracing::~RayTracing()
{
    delete workerDistributor;
    delete rtRess;
    if (lightPerColumn != nullptr) delete[] lightPerColumn;
}

void RayTracing::connectResultReady(std::function<void()> callback)
{
    resultReadyCallback = callback;
}

void RayTracing::onRayWorkerReady(const int &x, const int &nbColumns, const PixScreen<ColorLight> &c, const int *totalLight)
{
    processFinished += nbColumns;
    for (int i = 0; i < nbColumns; i++) {
        lightPerColumn[x + i] = totalLight[i];
        colors.setColumn(x + i, c.getColumn(i));
    }

    if (processFinished == processWidth) {
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

    processWidth = calcSize.cx;
    processFinished = 0;
    processForUpdate = RAYTRACING::RefreshColumn;

    if (calcSize.cx != colors.width() || calcSize.cy != colors.height()) {
        // il ne faut pas en attribuer trop sinon trop de colonnes pour un thread
        // 1920 =>36 colonnes par worker
        // 150 => 10 colonnes par worker
        int columnsPerWorker = 20 * std::sqrt(calcSize.cx) / RAYTRACING::WorkerThread;
        std::cout << "ColumnsPerWorker " << columnsPerWorker << " Size(" << calcSize.cx << ", " << calcSize.cy << ") " << WorkerThread << std::endl;

        for (int i = 0; i < workerDistributor->nb_workers; i++) {
            workerDistributor->getWorkers()[i].setPrimaryWork(calcSize, columnsPerWorker);
        }
        colors = PixScreen<ColorLight>(calcSize);
        if (lightPerColumn != nullptr) delete[] lightPerColumn;
        lightPerColumn = new int[processWidth];
        for (int i = 0; i < processWidth; i++)
            lightPerColumn[i] = 0;
        image = Image(calcSize, Image::Format_RGBA32);
    }

#ifdef REFRESH_COLUMN
    for (int x = 0; x < image.width(); x++) {
        image.setPixelColor(x, 0, {255, 255, 255, 0});
        image.setPixelColor(x, 1, {255, 255, 255, 0});
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
    int64_t start = dt.getCurrent();
    int x, y;
    const double totalLight = calcTotalLight(); // < 1 ms

    uchar *pixelRef = image(0, 0);
    const int refToNextLine = 4 * (colors.width() - 1) + 1; // skip alpha and width pixels
    const int refToNextColumn = image.getDataLength() - 4;  // move height lines above, then to the pixel on the right

#if PIXSCREEN == 1 && true
    QList<ColorLight>::const_iterator cl;

    // 8 ms en full screen avec PixScreenQList
    for (x = 0; x < colors.width(); x++) {
        cl = colors[x].cbegin();
#else
    const ColorLight *cl;

    // 10 ms en full screen avec PixScreenQList
    // 8 ms en full screen avec PixScreenT
    for (x = 0; x < colors.width(); x++) {
        cl = &colors[x][0];
#endif
        for (y = 0; y < colors.height(); y++, cl++) {
            *pixelRef++ = cl->redReduced(totalLight);
            *pixelRef++ = cl->greenReduced(totalLight);
            *pixelRef++ = cl->blueReduced(totalLight);
            pixelRef += refToNextLine; // go to (x,y+1)
        }
        pixelRef -= refToNextColumn; // go to (x+1,0)
#if PIXSCREEN == 1 && true
    }
#else
    }
#endif

    dt.addValue("paint", dt.getCurrent() - start);
    resultReadyCallback();
}

void RayTracing::onAllWorkersFinished()
{
    std::cout << "All ray workers have finished, processFinished : " << processFinished << "/" << processWidth << std::endl;
    workerDistributor->stop();
    paint();
    dt.addValue("run", dt.getCurrent() - startRun);
    // std::cout << "GUIWorker::run #end " << dt << std::endl;
}
