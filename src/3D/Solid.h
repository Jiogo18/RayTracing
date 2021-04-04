#ifndef SOLID_H
#define SOLID_H

#include "Object3D.h"

namespace SOLID {
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

    QString getFileTexture(SOLID::Material material, QList<SOLID::Variation> variations);
    int getLight(SOLID::Material material, QList<SOLID::Variation> variations);
    constexpr inline float getSpeedOfLight(SOLID::Material material)
    {
        switch (material) {
        case SOLID::Material::air:
            return 1;
        case SOLID::Material::watter:
            return 0.75; // 1/1.33
        case SOLID::Material::glass:
        case SOLID::Material::green_glass:
        case SOLID::Material::mirror:
            return 0.667; // 1/1.5
        default:
            return 0;
        }
    } // fraction de C

}; // namespace SOLID

class Face : public Object
{
public:
    Face();
    Face(const Point3D &point, const HRect3D &rect, bool orientation,
         SOLID::Material material, QList<SOLID::Variation> variations);
    Face(const Face &face);
    HRect3D getMaxGeometry() const override { return maxGeometry; }
    Point3D getMiddleGeometry() const { return middleGeometry; }
    const Plan *getPlan() const { return &plan; }
    const QPointF &getPointC() const { return pC; }
    bool getOrientation() const { return orientation; }
    bool isValid() const { return material != SOLID::Material::none; }
    ColorLight getColor(const QPointF &point) const;
    const QString &getTexture() const { return texture; }
    const QImage *getTextureImg() const { return textureImg; }
    SOLID::Material getMaterial() const { return material; }
    Rot3D boundRot(const Rot3D &rot) const;
    Rot3D refractRot(const Rot3D &pos, float indiceRefrac) const;
    bool containsPoint(const Point3D &point) const override;

private:
    HRect3D rect;
    HRect3D maxGeometry;
    Point3D middleGeometry;
    void calcFace();
    SOLID::Material material;
    QList<SOLID::Variation> variations;
    QString texture;
    const QImage *textureImg = nullptr;
    Plan plan;
    QPointF pC;
    bool orientation; // true dans le sens positif du plan
};

class Solid : public Object
{
public:
    Solid(Pos3D pos, SOLID::Material material, QList<Face>);
    ~Solid() override;

    virtual HRect3D getSolidGeometry() const { return HRect3D{}; }
    HRect3D getMaxGeometry() const override { return getSolidGeometry() + getPoint(); }
    Point3D getMiddleGeometry() const { return getMaxGeometry().getMiddle(); }
    const QList<Face> *getFaces() const { return &faces; }

    SOLID::Material getMaterial() const { return material; }

    bool containsPoint(const Point3D &point) const override { return getMaxGeometry().contains(point); }

private:
    SOLID::Material material;
    QList<Face> faces;
};

class Block : public Solid
{
    // any Cuboid (pavé droit)
public:
    Block(Pos3D pos, Size3D size, SOLID::Material material);
    HRect3D getMaxGeometry() const override { return HRect3D{getPoint(), size}; }
    HRect3D getSolidGeometry() const override { return HRect3D{Point3D{0, 0, 0}, size}; }
    bool containsPoint(const Point3D &point) const override;

private:
    Size3D size;
    static QList<Face> createDefaultFaces(Point3D posSolid, Size3D size, SOLID::Material material);
};

class Cube : public Block
{
public:
    Cube(Pos3D pos, SOLID::Material material);
    Cube(Pos3D pos, SOLID::Material material, doubli scale);
};

class HalfCube : public Block
{
public:
    HalfCube(Pos3D pos, SOLID::Material material);
    HalfCube(Pos3D pos, SOLID::Material material, doubli scale);
    HRect3D getMaxGeometry() const override { return HRect3D{getPoint(), Size3D{1, 1, 0.5}}; }
};

#endif // SOLID_H
