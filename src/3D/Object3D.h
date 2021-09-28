#ifndef OBJECT3D_H
#define OBJECT3D_H

#include "Geometry3D.h"
#include "../RayTracing/ColorLight.h"
#include "../Qt_compat/QImage.h"
#include <map>

namespace OBJECT3D {
    static std::map<std::string, QImage> loadedTextures;
    static QImage missingTexture;

    const QImage *getTexture(const std::string &file);

} // namespace OBJECT3D
using namespace OBJECT3D;

/*****************************************************************************
  Object : anything wich is an object in the world
 *****************************************************************************/

class Object : public Pos3D
{
public:
    constexpr inline Object(const Pos3D &pos) : Pos3D(pos), maxGeometry(pos, Size3D(0, 0, 0)) {}
    constexpr inline Object(const Pos3D &pos, const HRect3D &maxGeometry) : Pos3D(pos), maxGeometry(maxGeometry) {}
    constexpr inline Object(const Object &obj) : Pos3D(obj), maxGeometry(obj.maxGeometry) {}
    virtual ~Object() {}
    Object *operator=(const Object &obj);

    constexpr inline const HRect3D &getMaxGeometry() const { return maxGeometry; }
    constexpr inline const Point3D &getMiddleGeometry() const { return maxGeometry; }
    virtual bool containsPoint(const Point3D &point) const { return maxGeometry.contains(point); }

protected:
    HRect3D maxGeometry;
};

#endif // OBJECT3D_H
