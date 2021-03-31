#ifndef RAYTRACING_H
#define RAYTRACING_H

#include "3DObject/Geometry3D.h"
#include "3DObject/World3D.h"
#include <QVector>
#include <QPainter>
#include <QPointer>
#include <QThread>
#include "map3D.h"

#include "DebugTime.h"

#define REFRESH_COLUMN // TODO: remove it
//#define DISABLE_RAYPROCESS

namespace RAYTRACING {
    const doubli viewDistance = 100;
    const radian angleH = degreesToRadians(120); // de 156° à 172° max pour un Humain (1 oeil)
    const radian angleV = degreesToRadians(100); // 180° = inf, 0° = 0
    const doubli xMax = tan(RAYTRACING::angleH / 2);
    const doubli yMax = tan(RAYTRACING::angleV / 2);
    const int pppH = 1;
    const int pppV = 1;
    const int ppp = pppV * pppH;
    const int WorkerThread = std::thread::hardware_concurrency() * 2;
    const int RefreshColumn = 200;
    const int gamma = 2; // TODO: réduire modifier ça lorsqu'on aura la lumière du soleil
} // namespace RAYTRACING
using namespace RAYTRACING;

template<typename T>
class PixScreen
{
public: // il faut au moins un PixScreen<T> dans ce fichier pour en avoir avec le même type dans d'autres fichiers
    constexpr inline PixScreen() : screen(0) {}
    constexpr inline PixScreen(const QSize &size) : screen(size.width()), h(size.height())
    {
        for (int i = 0; i < width(); i++)
            screen[i] = QList<T>(h);
    }
    constexpr inline PixScreen(const int &width, const int &height) : screen(width), h(height)
    {
        for (int i = 0; i < width; i++)
            screen[i] = QList<T>(h);
    }
    constexpr inline PixScreen *operator=(const PixScreen &ps)
    {
        screen = ps.screen;
        h = ps.h;
        return this;
    }

    constexpr inline void set(const int &x, const int &y, const T &pos) { screen[x][y] = pos; }
    constexpr inline const T &at(const int &x, const int &y) const { return screen[x][y]; }
    T &operator()(const int &x, const int &y) { return screen[x][y]; }

    constexpr inline int width() const { return screen.size(); }
    constexpr inline const int &height() const { return h; }
    constexpr inline QSize size() const { return QSize(width(), h); }
    constexpr inline int rowNumber() const { return width() * h; }

    constexpr inline QList<T> &operator[](const int &x) { return screen[x]; }
    constexpr inline const QList<T> &getColumn(int x) const { return screen.at(x); }
    constexpr inline void setColumn(const int &x, const QList<T> &column) { screen[x] = column; }

private:
    QList<QList<T>> screen;
    int h;
};

class RayTracingRessources
{
public:
    RayTracingRessources(const World *world, const Entity *client, DebugTime *dt);
    ~RayTracingRessources();
    const World *world = nullptr;
    DebugTime *dt = nullptr;
    void onWorldChanged();
    Pos3D clientPos;
    QMap<QString, const QImage *> *facesImg = nullptr;
    BLOCK::Material insideMaterial;
    void resetRessources(const Entity *client);
};

class Ray
{
public:
    Ray(const Pos3D &pos, RayTracingRessources *rtRess);
    ColorLight getColor() const;
    void process(const World *world);

private:
    Pos3D pos;
    QList<ColorLight> colors;
    const Face *lastFace = nullptr;
    bool lastMoved = false;
    int opacity = 0;
    // vector objTraverse;
    doubli distParcouru;
    void moveTo(const Point3D &newPos);
    const Face *getFirstIntersection(const World *world, Point3D &pInterMin) const;
    // black list (et white list pour transparence) du dernier objet traversé
    // par défaut: sortie du client (pour pas le retapper)
    // si rebond: dernier objet en mode "sortie"
    // si entrée (transparent): dernier objet en mode "entrée" puis "sortie" pour calculer l'épaisseur
    // si un objet dans l'objet entré est touché alors on sort du premier objet au point P et on rentre ds le 2
    // puis on resort du 2 on rerentre ds le 1 et resort du 1 (de l'autre coté de 2)
    bool enter = false;
    BLOCK::Material insideMaterial;
    RayTracingRessources *rtRess = nullptr;
    const QImage *getTexture(const QString &file) const;
};

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
    void run() override; // process ONE column at a time
    int workerId;
    QSize sceneSize;

    int xScene; // just ONE column, more columns with RAYTRACING::ColumnsPerWorker
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
};

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
