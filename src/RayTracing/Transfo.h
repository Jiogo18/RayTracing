#ifndef TRANSFO_H
#define TRANSFO_H

#include "src/3D/Geometry3D.h"
#include "src/World/World3D.h"

namespace Transfo3D {
    // vérifie l'ordre de grandeur pouvant poser des pbs
    // World changeRef(const World *world, Pos3D pos);
    // pas besion de perspective si il y a le ray tracing
    // void addPerspective(World *world, double persp, double zoom);
    // Point3D pointPerspective(Point3D point, double perspective, double zoom);

    Rot3D refractRot(const Rot3D &posOrigin, const Rot3D &pos, float indiceRefrac);
} // namespace Transfo3D

#endif // TRANSFO_H
