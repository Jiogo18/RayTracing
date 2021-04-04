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

class Object : public Pos3D, public QObject
{
public:
    Object(const Pos3D &pos);
    Object(const Object &obj);
    virtual ~Object();
    Object *operator=(const Object &obj);

    Point3D getPoint() const { return static_cast<Point3D>(*this); }
    Pos3D getPos() const { return static_cast<Pos3D>(*this); }
    void setPoint(const Point3D &point) { Point3D::operator=(point); }
    void setPos(const Pos3D &pos) { Pos3D::operator=(pos); }

    virtual HRect3D getMaxGeometry() const { return HRect3D{getPoint(), getPoint()}; }
    virtual bool containsPoint(const Point3D &point) const { return point == getPoint(); }
};

#endif // OBJECT3D_H
