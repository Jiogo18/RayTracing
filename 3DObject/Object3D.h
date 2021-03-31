#ifndef OBJECT3D_H
#define OBJECT3D_H

#include "Geometry3D.h"
#include <QImage>
#include <QMap>

namespace BLOCK {
    enum Material {
        none,
        air,
        oak_log,
        birch_log,
        stone,
        glowstone,
        glass,
        green_glass,
        mirror,
        watter,
    };
    enum Variation {
        front,
        back,
        left,
        right,
        top,
        bottom
    };
}; // namespace BLOCK
namespace ENTITY {
    enum Type {
        none,
        player,
        ghost //spectator
    };
    enum Direction { front,
                     back,
                     left,
                     right,
                     top,
                     bottom };
    doubli baseSpeed(ENTITY::Type type, ENTITY::Direction dir);
} // namespace ENTITY

namespace OBJECT3D {
    QString getFileTexture(BLOCK::Material material, QList<BLOCK::Variation> variations);
    QImage getTexture(QString file);
    int getLight(BLOCK::Material material, QList<BLOCK::Variation> variations);
    float getSpeedOfLight(BLOCK::Material material); // fraction de C
} // namespace OBJECT3D
using namespace OBJECT3D;

class ColorLight
{
public:
    ColorLight();
    ColorLight(int r, int g, int b, int a, int light = 0);
    ColorLight(const ColorLight &color);
    QColor getColorReduced(double reduce) const;
    inline int red() const { return r; }
    inline int green() const { return g; }
    inline int blue() const { return b; }
    inline int alpha() const { return a; }
    inline int getLight() const { return light; }
    void operator+=(const ColorLight &color); //couleur après + couleur avant = couleur mixé
    ColorLight *operator=(const ColorLight &color);

private:
    //entre 0 et 255
    int r, g, b, a;
    int light; //entre 0 et infini (0 par défaut et valeur >0 si on arrive à une source de lumière)
    //bc de light pr le soleil, les sources concentrées (plusieurs Ray)
    inline int colorReduced(int c, double reduce) const
    {
        c = c * light / reduce;
        return c >= 255 ? 255 : c;
    }
};

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

    virtual HRect3D getMaxGeometry() const { return HRect3D(getPoint(), getPoint()); }
    virtual bool containsPoint(const Point3D &point) const { return point == getPoint(); }
    virtual BLOCK::Material getMaterial() const { return BLOCK::Material::none; }
};

class Face : public Object
{
public:
    Face();
    Face(const Point3D &point, const HRect3D &rect, bool orientation,
         BLOCK::Material material, QList<BLOCK::Variation> variations);
    Face(const Face &face);
    HRect3D getMaxGeometry() const override { return maxGeometry; }
    Point3D getMiddleGeometry() const { return middleGeometry; }
    const Plan *getPlan() const { return &plan; }
    const QPointF &getPointC() const { return pC; }
    bool getOrientation() const { return orientation; }
    bool isValid() const { return material != BLOCK::Material::none; }
    ColorLight getColor(const QPointF &point, const QImage *img) const;
    QString getTexture() const { return texture; }
    BLOCK::Material getMaterial() const override { return material; }
    Rot3D boundRot(const Rot3D &rot) const;
    Rot3D refractRot(const Rot3D &pos, float indiceRefrac) const;
    bool containsPoint(const Point3D &point) const override;

private:
    HRect3D rect;
    HRect3D maxGeometry;
    Point3D middleGeometry;
    void calcFace();
    BLOCK::Material material;
    QList<BLOCK::Variation> variations;
    QString texture;
    Plan plan;
    QPointF pC;
    bool orientation; // true dans le sens positif du plan
};

class Solid : public Object
{
public:
    Solid(Pos3D pos, BLOCK::Material material, QList<Face>);
    ~Solid() override;

    virtual HRect3D getSolidGeometry() const { return HRect3D(); }
    HRect3D getMaxGeometry() const override { return getSolidGeometry() + getPoint(); }
    Point3D getMiddleGeometry() const { return getMaxGeometry().getMiddle(); }
    const QList<Face> *getFaces() const { return &faces; }

    BLOCK::Material getMaterial() const override { return material; }

    bool containsPoint(const Point3D &point) const override { return getMaxGeometry().contains(point); }

private:
    BLOCK::Material material;
    QList<Face> faces;
};

class Block : public Solid
{
    // any Cuboid (pavé droit)
public:
    Block(Pos3D pos, Size3D size, BLOCK::Material material);
    HRect3D getMaxGeometry() const override { return HRect3D(getPoint(), size); }
    HRect3D getSolidGeometry() const override { return HRect3D(Point3D(0, 0, 0), size); }
    bool containsPoint(const Point3D &point) const override;

private:
    Size3D size;
    static QList<Face> createDefaultFaces(Point3D posSolid, Size3D size, BLOCK::Material material);
};

class Cube : public Block
{
public:
    Cube(Pos3D pos, BLOCK::Material material);
    Cube(Pos3D pos, BLOCK::Material material, doubli scale);
};

class HalfCube : public Block
{
public:
    HalfCube(Pos3D pos, BLOCK::Material material);
    HalfCube(Pos3D pos, BLOCK::Material material, doubli scale);
    HRect3D getMaxGeometry() const override { return HRect3D(getPoint(), Size3D(1, 1, 0.5)); }
};

class EntityAttribute
{
public:
    EntityAttribute() { type = ENTITY::Type::none; }
    EntityAttribute(ENTITY::Type type);
    EntityAttribute(const EntityAttribute &attribute);
    void setType(ENTITY::Type type) { this->type = type; }
    ENTITY::Type getType() const { return type; }
    doubli getSpeed(ENTITY::Direction dir) const;
    void setSpeed(ENTITY::Direction dir, doubli speed)
    {
        this->speed.remove(dir);
        this->speed.insert(dir, speed);
    }
    doubli getSpeedF() const { return getSpeed(ENTITY::Direction::front); }
    doubli getSpeedB() const { return getSpeed(ENTITY::Direction::back); }
    doubli getSpeedL() const { return getSpeed(ENTITY::Direction::left); }
    doubli getSpeedR() const { return getSpeed(ENTITY::Direction::right); }
    doubli getSpeedU() const { return getSpeed(ENTITY::Direction::top); }
    doubli getSpeedD() const { return getSpeed(ENTITY::Direction::bottom); }

private:
    QMap<int, doubli> speed;
    ENTITY::Type type;
};

class Entity : public Object
{
public:
    Entity(const Pos3D &pos, ENTITY::Type type);

    void moveUp() { addZ(attribute.getSpeedU()); }
    void moveDown() { addZ(-attribute.getSpeedD()); }
    void moveFront() { moveWithRot(attribute.getSpeedF(), 0); }
    void moveBack() { moveWithRot(attribute.getSpeedB(), M_PI); }
    void moveLeft() { moveWithRot(attribute.getSpeedL(), -M_PI / 2); }
    void moveRight() { moveWithRot(attribute.getSpeedR(), M_PI / 2); }
    void moveTo(const Point3D &point) { setPoint(point); }
    void moveTo(const Pos3D &pos)
    {
        moveTo(static_cast<Point3D>(pos));
        setRX(pos.rX());
        setRZ(pos.rZ());
    }
    void moveRX(radian rX) { addRX(rX); }
    void moveRZ(radian rZ) { addRZ(rZ); }

private:
    EntityAttribute attribute;
};

#endif // OBJECT3D_H
