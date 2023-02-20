#include "RayTracing.h"
#include "GPUCalls.h"

RayTracing::RayTracing(const map3D *map) : Thread(), map(map)
{
    rtRess = new RayTracingRessources(map->getWorld(), map->getClient(), &dt);
    // connect(map->getWorld(), &World::changed, this, &RayTracing::onWorldChanged);
}

RayTracing::~RayTracing()
{
    delete rtRess;
}

void RayTracing::connectResultReady(std::function<void()> callback)
{
    resultReadyCallback = callback;
}

void RayTracing::run()
{
    dt.clear();
    startRun = dt.getCurrent();
    rtRess->resetRessources(map->getClient()); // reset the pos

    if (calcSize.cx != colorLightImage.width() || calcSize.cy != colorLightImage.height()) {
        colorLightImage = PixScreen<ColorLight>(calcSize);
        image = Image(calcSize, Image::Format_RGBA32);

        onSizeChanged();
    }

    startGeneration();
}

void RayTracing::onImageReady()
{
    resultReadyCallback();
}

void RayTracing::onGenerationFinished()
{
    onImageReady();
    // dt.addValue("run", dt.getCurrent() - startRun);
    // std::cout << "GUIWorker::run #end " << dt << std::endl;
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

void RayTracing::updateImage()
{
    // int64_t start = dt.getCurrent();
    int x, y;
    const double totalLight = calcTotalLight(); // < 1 ms

    uchar *pixelRef = image(0, 0);
    const int refToNextLine = 4 * (colorLightImage.width() - 1) + 1; // skip alpha and width pixels
    const int refToNextColumn = image.getDataLength() - 4;           // move height lines above, then to the pixel on the right

#if PIXSCREEN == 1 && true
    QList<ColorLight>::const_iterator cl;

    // 8 ms en full screen avec PixScreenQList
    for (x = 0; x < colorLightImage.width(); x++) {
        cl = colorLightImage[x].cbegin();
#else
    const ColorLight *cl;

    // 10 ms en full screen avec PixScreenQList
    // 8 ms en full screen avec PixScreenT
    for (x = 0; x < colorLightImage.width(); x++) {
        cl = &colorLightImage[x][0];
#endif
        for (y = 0; y < colorLightImage.height(); y++, cl++) {
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

    // dt.addValue("updateImage", dt.getCurrent() - start);
}

RayTracingCPU::RayTracingCPU(const map3D *map) : RayTracing(map)
{
    workerDistributor = new RayTracingDistributor(rtRess);
    RayTracingWorker *rayWorkers = workerDistributor->getWorkers();
    for (int i = 0; i < workerDistributor->nb_workers; i++) {
        rayWorkers[i].connectResultReady([this](int x, int nbColumns, const PixScreen<ColorLight> &c, const int *totalLight) { this->onRayWorkerReady(x, nbColumns, c, totalLight); });
    }
}

RayTracingCPU::~RayTracingCPU()
{
    delete workerDistributor;
    if (lightPerColumn != nullptr) delete[] lightPerColumn;
}

void RayTracingCPU::onSizeChanged()
{
    processWidth = calcSize.cx;

    // il ne faut pas en attribuer trop sinon trop de colonnes pour un thread
    // 1920 =>36 colonnes par worker
    // 150 => 10 colonnes par worker
    int columnsPerWorker = 20 * std::sqrt(calcSize.cx) / RAYTRACING::WorkerThread;
    std::cout << "ColumnsPerWorker " << columnsPerWorker << " Size(" << calcSize.cx << ", " << calcSize.cy << ") " << WorkerThread << std::endl;

    for (int i = 0; i < workerDistributor->nb_workers; i++) {
        workerDistributor->getWorkers()[i].setPrimaryWork(calcSize, columnsPerWorker);
    }
    if (lightPerColumn != nullptr) delete[] lightPerColumn;
    lightPerColumn = new int[processWidth];
    for (int i = 0; i < processWidth; i++)
        lightPerColumn[i] = 0;
}

void RayTracingCPU::startGeneration()
{
    processWidth = calcSize.cx;
    processFinished = 0;
    processForUpdate = RAYTRACING::RefreshColumn;

#ifdef REFRESH_COLUMN
    for (int x = 0; x < image.width(); x++) {
        image.setPixelColor(x, 0, {255, 255, 255, 0});
        image.setPixelColor(x, 1, {255, 255, 255, 0});
    }
#endif // REFRESH_COLUMN

#ifdef DISABLE_WORKER_DISTRIBUTOR
    onAllWorkersFinished();
#else
    workerDistributor->start(processWidth);
#endif // DISABLE_WORKER_DISTRIBUTOR
}

constexpr double RayTracingCPU::calcTotalLight() const
{
    double totalLight = 0; // une sorte de moyenne pour privilégier le poids des très lumineux
    for (int i = 0; i < processWidth; i++)
        totalLight += lightPerColumn[i];
    return totalLight / colorLightImage.rowNumber(); // moyenne par pixel
    // full screen : 2 msec / 20 appels
}

void RayTracingCPU::onRayWorkerReady(const int &x, const int &nbColumns, const PixScreen<ColorLight> &c, const int *totalLight)
{
    processFinished += nbColumns;
    mutexWorkerFinished.lock();
    for (int i = 0; i < nbColumns; i++) {
        lightPerColumn[x + i] = totalLight[i];
        colorLightImage.setColumn(x + i, c.getColumn(i));
    }
    mutexWorkerFinished.unlock();

    if (processFinished == processWidth) {
        onAllWorkersFinished();
    }
#ifdef REFRESH_COLUMN
    else {
        if (processForUpdate <= processFinished) {
            processForUpdate = processFinished + RAYTRACING::RefreshColumn;
            updateImage();
            onImageReady();
        }
    }
#endif // REFRESH_COLUMN
}

void RayTracingCPU::onAllWorkersFinished()
{
    std::cout << "All ray workers have finished, processFinished : " << processFinished << "/" << processWidth << std::endl;
    workerDistributor->stop();
    updateImage();
    onGenerationFinished();
}

RayTracingGPU::RayTracingGPU(const map3D *map) : RayTracing(map)
{
    timerFinished.connectOnTimeout([this]() { this->onGenerationFinished(); });
    timerFinished.setSingleShot(true);
}

RayTracingGPU::~RayTracingGPU()
{
    timerFinished.stop();
}

void RayTracingGPU::onSizeChanged()
{
}

void RayTracingGPU::startGeneration()
{
    long long start = dt.getCurrent();
    GPUCalls::calcImage(map, calcSize, image.data);
    long long end = dt.getCurrent();
    std::cout << "Image generated using GPU in " << (end - start) << " us" << std::endl;

    timerFinished.start(1); // Call onGenerationFinished() after 1 ms
}

constexpr double RayTracingGPU::calcTotalLight() const
{
    return 1;
}
