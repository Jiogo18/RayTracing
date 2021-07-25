#ifndef RESSOURCES_H
#define RESSOURCES_H

#include <QList>
#include "src/World/map3D.h"
#include "src/DebugTime.h"

//#define REFRESH_COLUMN // TODO: remove it
//#define DISABLE_RAYPROCESS
//#define DISABLE_WORKER_DISTRIBUTOR

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

class RayTracingRessources
{
public:
    RayTracingRessources(const World *world, const Entity *client, DebugTime *dt);
    const World *world = nullptr;
    DebugTime *dt = nullptr;
    void onWorldChanged();
    Pos3D clientPos;
    SOLID::Material insideMaterial;
    void resetRessources(const Entity *client);
};

#endif // RESSOURCES_H
