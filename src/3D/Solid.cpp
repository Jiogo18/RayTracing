#include "Solid.h"

std::string SOLID::getFileTexture(SOLID::Material material, SOLID::Variation variations)
{
    std::string texture = "";
    switch (material) {
    case SOLID::Material::none:
        texture = "blocks/none";
        break;
    case SOLID::Material::air:
        texture = "blocks/air";
        break;
    case SOLID::Material::oak_log:
        texture = "blocks/oak_log";
        break;
    case SOLID::Material::birch_log:
        texture = "blocks/birch_log";
        break;
    case SOLID::Material::stone:
        texture = "blocks/stone";
        break;
    case SOLID::Material::glowstone:
        texture = "blocks/glowstone";
        break;
    case SOLID::Material::glass:
        texture = "blocks/glass";
        break;
    case SOLID::Material::green_glass:
        texture = "blocks/green_stained_glass";
        break;
    case SOLID::Material::mirror:
        texture = "blocks/mirror";
        break;
    case SOLID::Material::watter:
        texture = "blocks/watter";
        break;
    case SOLID::Material::hologramme:
        texture = "hologramme_";
        break;
    }
    switch (material) {
    case SOLID::Material::oak_log:
    case SOLID::Material::birch_log:
        if (variations == SOLID::Variation::top
            || variations == SOLID::Variation::bottom)
            texture += "_top";
        break;
    case SOLID::Material::hologramme: {
        int alpha = 0;
        if (variations == SOLID::Variation::BIN1) alpha += 1;
        if (variations == SOLID::Variation::BIN2) alpha += 2;
        if (variations == SOLID::Variation::BIN3) alpha += 4;
        if (variations == SOLID::Variation::BIN4) alpha += 8;
        if (variations == SOLID::Variation::BIN5) alpha += 16;
        if (variations == SOLID::Variation::BIN6) alpha += 32;
        if (variations == SOLID::Variation::BIN7) alpha += 64;
        if (variations == SOLID::Variation::BIN8) alpha += 128;
        texture += std::to_string(alpha);
        //if(alpha > 140) return getFileTexture(stone, {});
        //else if(alpha > 130) return getFileTexture(green_glass, {});
        //else if(alpha > 180) return getFileTexture(glass, {});
        break;
    }
    default:
        break;
    }

    return texture;
}

int SOLID::getLight(SOLID::Material material, SOLID::Variation variations)
{
    switch (material) {
    case SOLID::glowstone:
        return 5;
    default:
        return 0;
    }
}

/*****************************************************************************
  Face
 *****************************************************************************/

Face::Face(const Point3D &point,
           const HRect3D &solidGeometry, bool orientation,
           const SOLID::Material &material,
           SOLID::Variation variations) : SolidBase(Pos3D(point, 0, 0), HRect3D(solidGeometry + point), material),
                                          variations(variations), orientation(orientation)
{
    texture = SOLID::getFileTexture(material, variations);
    textureImg = OBJECT3D::getTexture(texture);
    if (textureImg != nullptr && textureImg->isNull()) textureImg = nullptr;

    plan = Plan(maxGeometry);

    pC = plan.projeteOrtho(maxGeometry.getPointMax()) - Point2D(1, 1);
}
Face::Face(const Face &face) : SolidBase(face),
                               variations(face.variations),
                               texture(face.texture),
                               textureImg(face.textureImg),
                               plan(face.plan),
                               pC(face.pC),
                               orientation(face.orientation)
{}

ColorLight Face::getColor(const Point2D &point) const
{
#define FacegetColor 1
#if FacegetColor == 1 // image
    if (textureImg != nullptr) {
        if (0 <= point.x() && point.x() <= 1 && 0 <= point.y() && point.y() <= 1) {
            // switch selon les positions ? => via fichier + peut etre
            return ColorLight{textureImg->pixelColor(min(point.x() * textureImg->width(), textureImg->width() - 1),
                                                     min(point.y() * textureImg->height(), textureImg->height() - 1)),
                              SOLID::getLight(getMaterial(), variations)};
        } else {
            std::cout << "Face::getColor point pas dans le cadre: " << point << std::endl;
            return ColorLight{textureImg->pixelColor(0, 0), SOLID::getLight(getMaterial(), variations)};
        }
    }
#elif FacegetColor == 2 // colored
    //int casesDone = 0;
    for (int i = 0; i < variations.size(); i++) {
        switch (variations.at(i)) {
        case SOLID::Variation::top:
            //if(casesDone & 1) continue;
            //casesDone = casesDone | 1;
            return ColorLight{100, 100, 200, 250, 1};
        case SOLID::Variation::bottom:
            //if(casesDone & 2) continue;
            //casesDone = casesDone | 2;
            return ColorLight{200, 200, 100, 250, 1};
        case SOLID::Variation::front:
            //if(casesDone & 4) continue;
            //casesDone = casesDone | 4;
            return ColorLight{200, 100, 100, 250, 1};
        case SOLID::Variation::back:
            //if(casesDone & 8) continue;
            //casesDone = casesDone | 8;
            return ColorLight{100, 200, 200, 250, 1};
        case SOLID::Variation::left:
            //if(casesDone & 16) continue;
            //casesDone = casesDone | 16;
            return ColorLight{100, 200, 100, 250, 1};
        case SOLID::Variation::right:
            //if(casesDone & 32) continue;
            //casesDone = casesDone | 32;
            return ColorLight{200, 100, 200, 250, 1};
        }
    }
#endif                  // FacegetColor
    return ColorLight{0, 0, 0, 255, 0};
}

Rot3D Face::boundRot(const Rot3D &rot) const
{
    Vec3D P = rot.toVector(), Ori = plan.normale();

    doubli k = P.produitScalaire(Ori); // `/ Ori.distanceOrigine()` normalement mais il est unitaire ici
    Point3D Op = Ori * (2 * k);        // projeté de P sur l'axe de Ori * 2

#ifdef NAN_VERIF
    if (Op.isInf() || P.isInf() || Op.isNan() || P.isNan()) {
        std::cout << "Face::boundRot P - Op is nan or inf " << Op << " " << k << " " << P << std::endl;
    }
#endif // NAN_VERIF

    return Rot3D::fromVector(P - Op);
}

Rot3D Face::refractRot(const Rot3D &pos, float indiceRefrac) const
{
    if (indiceRefrac == 1) return pos; // shortcut, speedOut/speedIn
    if (indiceRefrac == 0 || isnan(indiceRefrac)) return Rot3D();

    Vec3D Ori = plan.normale();
    if (!orientation) {
        Ori = -Ori;
    }
    Point3D delta = pos.toVector() - Ori;

    Point3D Mid = Ori + delta / indiceRefrac;

#ifdef NAN_VERIF
    if (Mid.isInf() || Mid.isNan()) {
        std::cout << "Face::refractRot Mid is nan or inf: " << Mid << " " << indiceRefrac << std::endl;
    }
#endif // NAN_VERIF

    return Rot3D::fromVector(Mid);
}

bool Face::containsPoint(const Point3D &point) const
{
    return plan.containsPoint(point); // TODO: par rapport aux points ?
}

/*****************************************************************************
  Block
 *****************************************************************************/

Block::Block(const Pos3D &pos, const Size3D &size, SOLID::Material material, const std::vector<SOLID::Variation> &globalVariations)
    : Solid(pos,
            HRect3D{pos, size},
            material,
            createDefaultFaces(pos, size, material, globalVariations)),
      size(size)
{}

std::vector<Face> Block::createDefaultFaces(const Point3D &posSolid, const Size3D &size, SOLID::Material material, const std::vector<SOLID::Variation> &globalVariations)
{
    return {
        Face(posSolid, HRect3D(Point3D(0, 0, -size.dZ() / 2), Size3D(size.dX(), size.dY(), 0)), 0, material, SOLID::Variation::bottom), // BOTTOM
        Face(posSolid, HRect3D(Point3D(0, 0, +size.dZ() / 2), Size3D(size.dX(), size.dY(), 0)), 1, material, SOLID::Variation::top),    // TOP
        Face(posSolid, HRect3D(Point3D(+size.dX() / 2, 0, 0), Size3D(0, size.dY(), size.dZ())), 1, material, SOLID::Variation::front),  // NORTH
        Face(posSolid, HRect3D(Point3D(-size.dX() / 2, 0, 0), Size3D(0, size.dY(), size.dZ())), 0, material, SOLID::Variation::back),   // SOUTH
        Face(posSolid, HRect3D(Point3D(0, -size.dY() / 2, 0), Size3D(size.dX(), 0, size.dZ())), 0, material, SOLID::Variation::right),  // EAST
        Face(posSolid, HRect3D(Point3D(0, +size.dY() / 2, 0), Size3D(size.dX(), 0, size.dZ())), 1, material, SOLID::Variation::left)    // WEST*/
    };
}

/*****************************************************************************
  Cube
 *****************************************************************************/

Cube::Cube(Pos3D pos, SOLID::Material material) : Block(pos, Size3D(1, 1, 1), material) {}

Cube::Cube(Pos3D pos, SOLID::Material material, doubli scale)
    : Block(pos, Size3D(scale, scale, scale), material) {}

/*****************************************************************************
  HalfCube
 *****************************************************************************/

HalfCube::HalfCube(Pos3D pos, SOLID::Material material) : Block(pos, Size3D(1, 1, 0.5), material) {}

HalfCube::HalfCube(Pos3D pos, SOLID::Material material, doubli scale)
    : Block(pos, Size3D(scale, scale, scale * 0.5), material) {}
