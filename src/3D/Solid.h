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

/*****************************************************************************
  BaseSolid : An object with some properties of a solid
 *****************************************************************************/

class SolidBase : public Object
{
public:
    constexpr inline SolidBase(const Pos3D &pos) : Object(pos) {}
    constexpr inline SolidBase(const Pos3D &pos, const SOLID::Material &material) : Object(pos), material(material) {}
    constexpr inline SolidBase(const Pos3D &pos, const HRect3D &maxGeometry) : Object(pos, maxGeometry) {}
    constexpr inline SolidBase(const Pos3D &pos, const HRect3D &maxGeometry, const SOLID::Material &material) : Object(pos, maxGeometry), material(material) {}
    constexpr inline SolidBase(const SolidBase &obj) : Object(obj), material(obj.material) {}

    constexpr inline const SOLID::Material &getMaterial() const { return material; }

protected:
    SOLID::Material material = SOLID::Material::none;
};

/*****************************************************************************
  Face : A face of a solid
 *****************************************************************************/

class Face : public SolidBase
{
public:
    Face();
    Face(const Point3D &point, const HRect3D &rect, bool orientation,
         const SOLID::Material &material, QList<SOLID::Variation> variations);
    Face(const Face &face);

    constexpr inline const Plan *getPlan() const { return &plan; }
    constexpr inline const QPointF &getPointC() const { return pC; }
    constexpr inline bool getOrientation() const { return orientation; }
    constexpr inline bool isValid() const { return material != SOLID::Material::none; }
    ColorLight getColor(const QPointF &point) const;
    constexpr inline const QString &getTexture() const { return texture; }
    constexpr inline const QImage *getTextureImg() const { return textureImg; }
    Rot3D boundRot(const Rot3D &rot) const;
    Rot3D refractRot(const Rot3D &pos, float indiceRefrac) const;
    bool containsPoint(const Point3D &point) const override;

private:
    HRect3D rect;
    void calcFace();
    QList<SOLID::Variation> variations;
    QString texture;
    const QImage *textureImg = nullptr;
    Plan plan;
    QPointF pC;
    bool orientation; // true dans le sens positif du plan
};

/*****************************************************************************
  Solid : any object with a volume
 *****************************************************************************/

class Solid : public SolidBase
{
public:
    Solid(const Pos3D &pos, const HRect3D &maxGeometry, const SOLID::Material &material, const QList<Face> &faces)
        : SolidBase(pos, maxGeometry, material), faces(faces) {}
    ~Solid() {}

    Point3D getMiddleGeometry() const { return maxGeometry.getMiddle(); }
    const QList<Face> *getFaces() const { return &faces; }

protected:
    QList<Face> faces;
};

/*****************************************************************************
  Block : Any Cuboid (pavé droit)
 *****************************************************************************/

class Block : public Solid
{
public:
    Block(const Pos3D &pos, const Size3D &size, SOLID::Material material);

private:
    Size3D size;
    static QList<Face> createDefaultFaces(const Point3D &posSolid, const Size3D &size, SOLID::Material material);
};

/*****************************************************************************
  Specials Blocks
   - Cube
   - HalfCube (horizontal)
 *****************************************************************************/

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
};

#endif // SOLID_H
