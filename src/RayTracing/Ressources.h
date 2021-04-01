#ifndef RESSOURCES_H
#define RESSOURCES_H

#include <QList>
#include "src/World/map3D.h"
#include "src/DebugTime.h"

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

#endif // RESSOURCES_H
