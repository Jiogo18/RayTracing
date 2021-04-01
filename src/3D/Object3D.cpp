#include "Object3D.h"

doubli ENTITY::baseSpeed(ENTITY::Type type, ENTITY::Direction dir)
{
    doubli speed = 0;
    switch (type) {
    case ENTITY::Type::none:
        speed = 0;
        break;
    case ENTITY::Type::player:
    case ENTITY::Type::ghost:
        speed = 0.1L;
        break;
    }
    switch (type) {
    case ENTITY::Type::player:
        switch (dir) {
        case ENTITY::Direction::left:
        case ENTITY::Direction::right:
        case ENTITY::Direction::back:
            speed /= 2;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    return roundNull(speed);
}

QString OBJECT3D::getFileTexture(BLOCK::Material material, QList<BLOCK::Variation> variations)
{
    QString texture = "";
    switch (material) {
    case BLOCK::Material::none:
        texture = "blocks/none";
        break;
    case BLOCK::Material::air:
        texture = "blocks/air";
        break;
    case BLOCK::Material::oak_log:
        texture = "blocks/oak_log";
        break;
    case BLOCK::Material::birch_log:
        texture = "blocks/birch_log";
        break;
    case BLOCK::Material::stone:
        texture = "blocks/stone";
        break;
    case BLOCK::Material::glowstone:
        texture = "blocks/glowstone";
        break;
    case BLOCK::Material::glass:
        texture = "blocks/glass";
        break;
    case BLOCK::Material::green_glass:
        texture = "blocks/green_stained_glass";
        break;
    case BLOCK::Material::mirror:
        texture = "blocks/mirror";
        break;
    case BLOCK::Material::watter:
        texture = "blocks/watter";
        break;
    }
    switch (material) {
    case BLOCK::Material::oak_log:
    case BLOCK::Material::birch_log:
        if (variations.contains(BLOCK::Variation::top)
            || variations.contains(BLOCK::Variation::bottom))
            texture += "_top";
        break;
    default:
        break;
    }

    return texture;
}
QImage OBJECT3D::getTexture(QString file)
{
    QImage img;
    if (img.load(":/ressourcepacks/default/textures/" + file + ".png")) {
        if (img.format() != QImage::Format_RGBA64) {
            img = img.convertToFormat(QImage::Format_RGBA64);
        }
        return img;
    }
    qWarning() << "[OBJECT3D::getTexture] can't load this texture:" << file;
    /*if(img.load(":/ressourcepacks/default/textures/blocks/block_nul.png"))
        return img;*/
    img = QImage(2, 2, QImage::Format_RGBA64);
    img.fill(QColor(0, 0, 0, 100));
    img.setPixelColor(0, 0, QColor(255, 0, 255, 100));
    img.setPixelColor(1, 1, QColor(255, 0, 255, 100));

    return img;
}
int OBJECT3D::getLight(BLOCK::Material material, QList<BLOCK::Variation> variations)
{
    Q_UNUSED(variations)
    switch (material) {
    case BLOCK::glowstone:
        return 5;
    default:
        return 0;
    }
}

float OBJECT3D::getSpeedOfLight(BLOCK::Material material)
{
    switch (material) {
    case BLOCK::Material::air:
        return 1;
    case BLOCK::Material::watter:
        return 0.75; // 1/1.33
    case BLOCK::Material::glass:
    case BLOCK::Material::green_glass:
    case BLOCK::Material::mirror:
        return 0.667; // 1/1.5
    default:
        return 0;
    }
}

Face::Face() : Object(Pos3D(0, 0, 0, 0, 0))
{
    rect = HRect3D(Point3D(0, 0, 0), Point3D(0, 0, 0));
    material = BLOCK::Material::none;
    variations.clear();
    variations.append(BLOCK::Variation::front);
    calcFace();
}
Face::Face(const Point3D &point, const HRect3D &rect, bool orientation,
           BLOCK::Material material, QList<BLOCK::Variation> variations) : Object(Pos3D(point, 0, 0))
{
    this->rect = rect;
    this->orientation = orientation;
    this->material = material;
    this->variations = variations;
    calcFace();
}
Face::Face(const Face &face) : Object(face)
{
    rect = face.rect;
    material = face.material;
    variations = face.variations;
    maxGeometry = face.maxGeometry;
    middleGeometry = face.middleGeometry;
    texture = face.texture;
    plan = face.plan;
    orientation = face.orientation;
    pC = face.pC;
}
ColorLight Face::getColor(const QPointF &point, const QImage *img) const
{
    if (img != nullptr && !img->isNull()) {
        QColor color;
        if (0 <= point.x() && point.x() <= 1 && 0 <= point.y() && point.y() <= 1) {
            // switch selon les positions ? => via fichier + peut etre
            int x = qFloor(point.x() * img->width());
            int y = qFloor(point.y() * img->height());
            if (img->width() <= x) x = img->width() - 1;
            if (img->height() <= y) y = img->height() - 1;
            color = img->pixelColor(x, y);
        } else {
            color = img->pixelColor(0, 0);
            qDebug() << "Face::getColor point pas dans le cadre:" << point;
        }
        return ColorLight(color.red(), color.green(), color.blue(), color.alpha(),
                          OBJECT3D::getLight(getMaterial(), variations));
    }

    /*int casesDone = 0;
    for(int i=0; i<variations.size(); i++) {
        switch (variations.at(i)) {
        case BLOCK::Variation::top:
            if(casesDone & 1) continue;
            casesDone = casesDone | 1;
            return ColorLight(100, 100, 200, 255, 1);
        case BLOCK::Variation::bottom:
            if(casesDone & 2) continue;
            casesDone = casesDone | 2;
            return ColorLight(200, 200, 100, 255, 1);
        case BLOCK::Variation::front:
            if(casesDone & 4) continue;
            casesDone = casesDone | 4;
            return ColorLight(200, 100, 100, 255, 1);
        case BLOCK::Variation::back:
            if(casesDone & 8) continue;
            casesDone = casesDone | 8;
            return ColorLight(100, 200, 200, 255, 1);
        case BLOCK::Variation::left:
            if(casesDone & 16) continue;
            casesDone = casesDone | 16;
            return ColorLight(100, 200, 100, 255, 1);
        case BLOCK::Variation::right:
            if(casesDone & 32) continue;
            casesDone = casesDone | 32;
            return ColorLight(200, 100, 200, 255, 1);
        }
    }*/
    return ColorLight(0, 0, 0, 255, 0);
}

Rot3D Face::boundRot(const Rot3D &rot) const
{
    Vec3D P = rot.toVector(), Ori = plan.normale();

    doubli k = P.produitScalaire(Ori); // `/ Ori.distanceOrigine()` normalement mais il est unitaire ici
    Point3D Op = Ori * (2 * k);        // projeté de P sur l'axe de Ori * 2

#ifdef NAN_VERIF
    if (Op.isInf() || P.isInf() || Op.isNan() || P.isNan()) {
        qDebug() << "Face::boundRot P - Op is nan or inf" << Op << k << P;
    }
#endif // NAN_VERIF

    return Rot3D::fromVector(P - Op);
}

Rot3D Face::refractRot(const Rot3D &pos, float indiceRefrac) const
{
    if (indiceRefrac == 1) return pos; // shortcut, speedOut/speedIn
    if (indiceRefrac == 0 || isnanf(indiceRefrac)) return Rot3D();

    Vec3D Ori = plan.normale();
    if (!orientation) {
        Ori = -Ori;
    }
    Point3D delta = pos.toVector() - Ori;

    Point3D Mid = Ori + delta / indiceRefrac;

#ifdef NAN_VERIF
    if (Mid.isInf() || Mid.isNan()) {
        qDebug() << "Face::refractRot Mid is nan or inf:" << Mid << indiceRefrac;
    }
#endif // NAN_VERIF

    return Rot3D::fromVector(Mid);
}

bool Face::containsPoint(const Point3D &point) const
{
    return plan.containsPoint(point); // TODO: par rapport aux points ?
}

void Face::calcFace()
{
    maxGeometry = rect + getPoint();
    middleGeometry = maxGeometry.getMiddle();
    texture = OBJECT3D::getFileTexture(material, variations);

    plan = Plan(maxGeometry);

    pC = plan.projeteOrtho(maxGeometry.getPointMax()) - QPointF(1, 1);
}

Object::Object(const Pos3D &pos) : Pos3D(pos), QObject() {}
Object::Object(const Object &obj) : Pos3D(obj), QObject() {}
Object::~Object() {}
Object *Object::operator=(const Object &obj)
{
    setPos(obj.getPos());
    return this;
}

Solid::Solid(Pos3D pos, BLOCK::Material material, QList<Face> faces) : Object(pos)
{
    this->material = material;
    this->faces = faces;
}
Solid::~Solid() {}

Block::Block(Pos3D pos, Size3D size, BLOCK::Material material)
    : Solid(pos, material, createDefaultFaces(pos, size, material)), size(size) {}

bool Block::containsPoint(const Point3D &point) const
{
    return HRect3D(getPoint(), size).contains(point);
    // TODO: prendre en compte la rotation
}

QList<Face> Block::createDefaultFaces(Point3D posSolid, Size3D size, BLOCK::Material material)
{
    return {
        Face(posSolid, HRect3D(Point3D(0, 0, 0), Point3D(size.dX(), size.dY(), 0)), 0, material, {BLOCK::Variation::bottom}), // BOTTOM
        Face(posSolid, HRect3D(Point3D(0, 0, size.dZ()), size.toPoint()), 1, material, {BLOCK::Variation::top}),              // TOP
        Face(posSolid, HRect3D(Point3D(0, 0, 0), Point3D(0, size.dY(), size.dZ())), 0, material, {BLOCK::Variation::back}),   // SOUTH
        Face(posSolid, HRect3D(Point3D(0, 0, 0), Point3D(size.dX(), 0, size.dZ())), 0, material, {BLOCK::Variation::right}),  // EAST
        Face(posSolid, HRect3D(Point3D(size.dX(), 0, 0), size.toPoint()), 1, material, {BLOCK::Variation::front}),            // NORTH
        Face(posSolid, HRect3D(Point3D(0, size.dY(), 0), size.toPoint()), 1, material, {BLOCK::Variation::left})              // WEST
    };
}

Cube::Cube(Pos3D pos, BLOCK::Material material) : Block(pos, Size3D(1, 1, 1), material) {}

Cube::Cube(Pos3D pos, BLOCK::Material material, doubli scale)
    : Block(pos, Size3D(scale, scale, scale), material) {}

HalfCube::HalfCube(Pos3D pos, BLOCK::Material material) : Block(pos, Size3D(1, 1, 0.5), material) {}

HalfCube::HalfCube(Pos3D pos, BLOCK::Material material, doubli scale)
    : Block(pos, Size3D(scale, scale, scale * 0.5), material) {}

EntityAttribute::EntityAttribute(ENTITY::Type type) { this->type = type; }
EntityAttribute::EntityAttribute(const EntityAttribute &obj)
{
    type = obj.type;
    speed = obj.speed;
}
doubli EntityAttribute::getSpeed(ENTITY::Direction dir) const
{
    if (speed.contains(dir))
        return speed.value(dir);
    else
        return baseSpeed(type, dir);
}

Entity::Entity(const Pos3D &pos, ENTITY::Type type) : Object(pos) { attribute.setType(type); }
