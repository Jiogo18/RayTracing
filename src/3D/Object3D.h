#ifndef OBJECT3D_H
#define OBJECT3D_H

#include "Geometry3D.h"
#include "src/RayTracing/ColorLight.h"
#include <QImage>
#include <QMap>

namespace OBJECT3D {
    static QMap<QString, QImage> loadedTextures;
    static QImage missingTexture;

    const QImage *getTexture(const QString &file);

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

    virtual HRect3D getMaxGeometry() const { return maxGeometry; }
    constexpr const Point3D &getMiddleGeometry() const { return middleGeometry; }
    virtual bool containsPoint(const Point3D &point) const { return maxGeometry.contains(point); }

protected:
    HRect3D maxGeometry;
    Point3D middleGeometry;
};

#endif // OBJECT3D_H
