#ifndef RAYTRACING_H
#define RAYTRACING_H

#include "Worker.h"

class RayTracing : public QThread
{
    Q_OBJECT
public:
    RayTracing(const map3D *map);
    ~RayTracing();

    RayTracing *setSize(const QSize &size)
    {
        if (!isRunning()) calcSize = size;
        return this;
    }
    const QImage &getImage() const { return image; }
    bool isRunning() { return QThread::isRunning() || workerDistributor->isRunning(); }

private slots:
    void onRayWorkerReady(const int &x, const int &nbColumns, const PixScreen<ColorLight> &c, const int *totalLight);
    void onWorldChanged(const WorldChange &change);

signals:
    void resultReady(const QImage &image);

private:
    void run() override;
    constexpr inline double calcTotalLight() const;
    void paint();
    void onAllWorkersFinished();

    RayTracingRessources *rtRess = nullptr;
    DebugTime dt;
    qint64 startRun;

    const map3D *map;
    QSize calcSize;

    PixScreen<ColorLight> colors;
    int *lightPerColumn = nullptr; // calculs des light de chaque colonne (en fonction de x)

    RayTracingDistributor *workerDistributor;
    int processWidth = 0;
    int processFinished = 0;
    int processForUpdate;

    QImage image;
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
