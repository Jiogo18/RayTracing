#ifndef OBJECT3D_H
#define OBJECT3D_H

#include "3DGeometry.h"
#include <QImage>
#include <QMap>

namespace BLOCK {
    enum Type {
        cube,
        slab
    };
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
        front, back, left, right, top, bottom
    };
};
namespace ENTITY {
    enum Type {
        none,
        player,
        ghost//spectator
    };
    enum Direction { front, back, left, right, top, bottom };
    doubli baseSpeed(ENTITY::Type type, ENTITY::Direction dir);
}

namespace OBJECT3D {
    QString getFileTexture(BLOCK::Material material, QList<BLOCK::Variation> variations);
    QImage getTexture(QString file);
    int getLight(BLOCK::Material material, QList<BLOCK::Variation> variations);
    float getSpeedOfLight(BLOCK::Material material);// fraction de C
}
using namespace OBJECT3D;


class ColorLight
{
public:
    ColorLight();
    ColorLight(int r, int g, int b, int a, int light = 0);
    ColorLight(const ColorLight &color);
    QColor getColor() const { return QColor(r, g, b); }//get color without alpha
    QColor getColorA() const { return QColor(r, g, b, a); }//get color with alpha
    QColor getColorReduced(double reduce) const;
    int getLight() const { return light; }
    void setRed(int r) { if(r<0) r=0; if(r>255) r=255; this->r = r; }
    void setGreen(int g) { if(g<0) g=0; if(g>255) g=255; this->g = g; }
    void setBlue(int b) { if(b<0) b=0; if(b>255) b=255; this->b = b; }
    void setAlpha(int a) { if(a<0) a=0; if(a>255) a=255; this->a = a; }
    void setLight(int light) { this->light = light; }
    ColorLight operator +(const ColorLight &color) const;//couleur devant + couleur après = couleur mixé
private:
    //entre 0 et 255
    int r;
    int g;
    int b;
    int a;//transparence ou reflexion !
    int light;//entre 0 et infini (0 par défaut et valeur >0 si on arrive à une source de lumière)
    //bc de light pr le soleil, les sources concentrées (plusieurs Ray)
};



class Object : public Pos3D, public QObject
{
public:
    Object(Pos3D pos);
    Object(const Object &obj);
    virtual ~Object();
    virtual HRect3D getMaxGeometry() const { return HRect3D(getPoint(), getPoint()); }
    Object *operator =(const Object &obj);
};



class Face : public Object
{
public:
    Face();
    Face(const Point3D &point, const HRect3D &rect, BLOCK::Material material, QList<BLOCK::Variation> variations);
    Face(const Face &face);
    HRect3D getMaxGeometry() const { return maxGeometry; }
    Point3D getMiddleGeometry() const { return middleGeometry; }
    bool isValid() const { return material != BLOCK::Material::none; }
    ColorLight getColor(const QPointF &point, const QImage *img) const;
    QString getTexture() const { return texture; }
    BLOCK::Material getMaterial() const { return material; }
    radiant boundRotX(const radiant &posRX) const;
    radiant boundRotZ(const radiant &posRZ) const;
    radiant refractRotX(const radiant &posRX, float speedIn, float speedOut) const;
    radiant refractRotZ(const radiant &posRZ, float speedIn, float speedOut) const;
private:
    HRect3D rect;
    HRect3D maxGeometry;
    Point3D middleGeometry;
    void calcFace();
    BLOCK::Material material;
    QList<BLOCK::Variation> variations;
    QString texture;
    radiant RX = 0;
    radiant RZ = 0;
};



class Block : public Object
{
public:
    Block(Pos3D pos, BLOCK::Type type, BLOCK::Material material);
    ~Block() override;

    static HRect3D getBlockGeometry(BLOCK::Type type);
    HRect3D getMaxGeometry() const override { return maxGeometry; }
    Point3D getMiddleGeometry() const { return middleMinGeometry; }
    const QList<Face*> *getFaces() const { return &faces; }

private:
    BLOCK::Type type;
    BLOCK::Material material;
    QList<Face*> faces;
    void deleteFace(Face *face);
    void calcFaces();
    HRect3D maxGeometry;
    Point3D middleMinGeometry;
    static QList<Face> getFaces(Point3D posBlock, BLOCK::Type type, BLOCK::Material material);
};





class EntityAttribute {
public:
    EntityAttribute() { type = ENTITY::Type::none; }
    EntityAttribute(ENTITY::Type type);
    EntityAttribute(const EntityAttribute &attribute);
    void setType(ENTITY::Type type) { this->type = type; }
    ENTITY::Type getType() const { return type; }
    doubli getSpeed(ENTITY::Direction dir) const;
    void setSpeed(ENTITY::Direction dir, doubli speed) { this->speed.remove(dir); this->speed.insert(dir, speed); }
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
    void moveLeft() { moveWithRot(attribute.getSpeedL(), -M_PI/2); }
    void moveRight() { moveWithRot(attribute.getSpeedR(), M_PI/2); }
    void moveTo(const Point3D &point) { setPoint(point); }
    void moveTo(const Pos3D &pos) { setPoint(pos.getPoint()); setRX(pos.getRX()); setRZ(pos.getRZ()); }
    void moveRX(radiant rX) { addRX(rX); }
    void moveRZ(radiant rZ) { addRZ(rZ); }
private:
    EntityAttribute attribute;
};


#endif // OBJECT3D_H
