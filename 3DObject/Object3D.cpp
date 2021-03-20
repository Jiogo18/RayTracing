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
    case BLOCK::Material::none: texture = "blocks/none"; break;
    case BLOCK::Material::air: texture = "blocks/air"; break;
    case BLOCK::Material::oak_log: texture = "blocks/oak_log"; break;
    case BLOCK::Material::birch_log: texture = "blocks/birch_log"; break;
    case BLOCK::Material::stone: texture = "blocks/stone"; break;
    case BLOCK::Material::glowstone: texture = "blocks/glowstone"; break;
    case BLOCK::Material::glass: texture = "blocks/glass"; break;
    case BLOCK::Material::green_glass: texture = "blocks/green_stained_glass"; break;
    case BLOCK::Material::mirror: texture ="blocks/mirror"; break;
    case BLOCK::Material::watter: texture ="blocks/watter"; break;
    }
    switch (material) {
    case BLOCK::Material::oak_log:
    case BLOCK::Material::birch_log:
        if(variations.contains(BLOCK::Variation::top) || variations.contains(BLOCK::Variation::bottom))
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
    if(img.load(":/ressourcepacks/default/textures/"+file+".png")) {
        if(img.format() != QImage::Format_RGBA64)
            img = img.convertToFormat(QImage::Format_RGBA64);
        return img;
    }
    qWarning() << "[OBJECT3D::getTexture] can't load this texture:" << file;
    /*if(img.load(":/ressourcepacks/default/textures/blocks/block_nul.png"))
        return img;*/
    img = QImage(2,2,QImage::Format_RGBA64);
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
    switch(material) {
    case BLOCK::Material::air:
        return 1;
    case BLOCK::Material::watter:
        return 0.75;// 1/1.33
    case BLOCK::Material::glass:
    case BLOCK::Material::green_glass:
    case BLOCK::Material::mirror:
        return 0.667;// 1/1.5
    default:
        return 0;
    }
}



ColorLight::ColorLight()
{
    this->light = 0;
}
ColorLight::ColorLight(int r, int g, int b, int a, int light)
{
    setRed(r); setGreen(g); setBlue(b); setAlpha(a);
    setLight(light);
}

ColorLight::ColorLight(const ColorLight &color)
{
    r = color.r; g = color.g; b = color.b; a = color.a;
    light = color.light;
}

QColor ColorLight::getColorReduced(double reduce) const
{
    int r = qFloor(this->r*light/reduce), g = qFloor(this->g*light/reduce), b = qFloor(this->b*light/reduce);
    if(r >= 255) r = 255;
    if(g >= 255) g = 255;
    if(b >= 255) b = 255;
    return QColor(r, g, b);
}

ColorLight ColorLight::operator +(const ColorLight &color) const
{
    //si color a une luminositée importante (plus que l'actuelle) alors color est +importante
    //si this a un alpha plus faible (transparent/miroir) alors color est +important
    if(a == 0)//this n'est absolument pas visible
        return color;
    double poids = (color.light - light + 1.0) * 255 / a;

    return ColorLight(qFloor((r + color.r*poids)/(1+poids)),
                      qFloor((g + color.g*poids)/(1+poids)),
                      qFloor((b + color.b*poids)/(1+poids)),
                      a + color.a,
                      light + color.light);
}





Face::Face() : Object(Pos3D(0, 0, 0, 0, 0))
{
    rect = HRect3D(Point3D(0, 0, 0), Point3D(0, 0, 0));
    material = BLOCK::Material::none;
    variations.clear();
    variations.append(BLOCK::Variation::front);
    calcFace();
}
Face::Face(const Point3D &point, const HRect3D &rect, BLOCK::Material material, QList<BLOCK::Variation> variations) : Object(Pos3D(point, 0, 0))
{
    this->rect = rect;
    this->material = material;
    this->variations = variations;
    calcFace();
}
Face::Face(const Face &face) : Object(face.getPos())
{
    rect = face.rect;
    material = face.material;
    variations = face.variations;
    maxGeometry = face.maxGeometry;
    middleGeometry = face.middleGeometry;
    texture = face.texture;
    RX = face.RX; RZ = face.RZ;
    plan = face.plan;
}
ColorLight Face::getColor(const QPointF &point, const QImage *img) const
{
    if(img != nullptr && !img->isNull()) {
        if(0 <= point.x() && point.x() <= 1 && 0 <= point.y() && point.y() <= 1) {
            //switch selon les positions ? => via fichier + peut etre
            int x = qFloor(point.x() * img->width());
            int y = qFloor(point.y() * img->height());
            if(img->width() <= x)
                x = img->width() - 1;
            if(img->height() <= y)
                y = img->height() - 1;
            QColor color = img->pixelColor(x, y);
            return ColorLight(color.red(), color.green(), color.blue(),
                              color.alpha(), OBJECT3D::getLight(material,
                                                                variations));
        }
        else
            qDebug() << "Face::getColor point pas dans le cadre:" << point;
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


radiant Face::boundRotX(const radiant &posRX) const
{
    //petit fix pour les faces du dessus mais il faudrait faire un autre calcul pour celles penchées
    if(variations.first() == BLOCK::Variation::top || variations.first() == BLOCK::Variation::top) {
        return RX*4 + posRX;//4+0+
    }
    return RX * 4 + M_PI - posRX;//4+M_PI-
}
radiant Face::boundRotZ(const radiant &posRZ) const
{
    return RZ * 4 - M_PI + posRZ;//*4-M_PI marche pour left et pour top
}

radiant Face::refractRotX(const radiant &posRX, float speedIn, float speedOut) const
{
    if(speedIn == speedOut) return posRX;
    // sin(angle in) / sin(angle out) = i out / i in
    // sin(angle out) = sin(angle in) * i in / i out = sin(angle in) * speed out / speed in
    return asin(sin(posRX + 2*RX) * speedOut / speedIn) - 2*RX; // +2RX : - 2RX
}

radiant Face::refractRotZ(const radiant &posRZ, float speedIn, float speedOut) const
{
    if(speedIn == speedOut) return posRZ;
    return asin(sin(posRZ) * speedOut / speedIn);//+ RZ - M_PI/4 : +0
}

void Face::calcFace()
{
    maxGeometry = rect + getPoint();
    middleGeometry = maxGeometry.getMiddle();
    texture = OBJECT3D::getFileTexture(material, variations);


    doubli deltaX = rect.getPointMax().getX() - rect.getPointMin().getX();
    doubli deltaY = rect.getPointMax().getY() - rect.getPointMin().getY();
    doubli deltaZ = rect.getPointMax().getZ() - rect.getPointMin().getZ();

    if(-0.5 < deltaX && deltaX < 0.5) {
        doubli deltaXZ = sqrt(deltaX * deltaX + deltaZ * deltaZ);
        RX = atan(deltaY / deltaXZ) + M_PI / 4;
    }
    else {
        doubli deltaYZ = sqrt(deltaY * deltaY + deltaZ * deltaZ);
        RX = atan(deltaX / deltaYZ);
    }
    // TODO: RX = atan(deltaX / deltaY);

    doubli deltaXY = sqrt(deltaX * deltaX + deltaY * deltaY);
    if(-0.5 < deltaXY && deltaXY < 0.5) {
        qDebug() << "alerte deltaZ trop faible" << deltaXY << deltaZ;
    }
    RZ = atan(deltaXY / deltaZ);

    plan = Plan(maxGeometry);
}



Object::Object(Pos3D pos) : Pos3D(pos), QObject() {}
Object::Object(const Object &obj) : Pos3D(obj.getPos()), QObject() { operator=(obj); }
Object::~Object() {}
Object *Object::operator =(const Object &obj) { setPos(obj.getPos()); return this; }



Block::Block(Pos3D pos, BLOCK::Type type, BLOCK::Material material) : Object(pos)
{
    this->type = type;
    this->material = material;
    calcFaces();
}
Block::~Block()
{
    while(!faces.isEmpty())
        deleteFace(faces.first());
}
HRect3D Block::getBlockGeometry(BLOCK::Type type)
{
    switch (type) {
    case BLOCK::Type::cube:
        return HRect3D(Point3D(0, 0, 0), Point3D(1, 1, 1));
    case BLOCK::Type::slab:
        return HRect3D(Point3D(0, 0, 0), Point3D(1, 1, 0.5L));
    }
    return HRect3D();
}

void Block::deleteFace(Face *face)
{
    faces.removeAll(face);
    if(face != nullptr)
        delete face;
}
void Block::calcFaces()
{
    QList<Face> faces = getFaces(this->getPoint(), type, material);
    while(!this->faces.isEmpty())
        deleteFace(this->faces.first());
    for(int i=0; i<faces.size(); i++)
        this->faces.append(new Face(faces.at(i)));
    maxGeometry = getBlockGeometry(type) + getPoint();
    middleMinGeometry = maxGeometry.getMiddle();
}
QList<Face> Block::getFaces(Point3D posBlock, BLOCK::Type type, BLOCK::Material material)
{
    switch (type) {
    case BLOCK::Type::cube:
        return {
            Face(posBlock, HRect3D(Point3D(0, 0, 0), Point3D(1, 1, 0)), material, {BLOCK::Variation::bottom}),//BOTTOM
            Face(posBlock, HRect3D(Point3D(0, 0, 1), Point3D(1, 1, 1)), material, {BLOCK::Variation::top}),//TOP
            Face(posBlock, HRect3D(Point3D(0, 0, 0), Point3D(0, 1, 1)), material, {BLOCK::Variation::back}),//SOUTH
            Face(posBlock, HRect3D(Point3D(0, 0, 0), Point3D(1, 0, 1)), material, {BLOCK::Variation::right}),//EAST
            Face(posBlock, HRect3D(Point3D(1, 0, 0), Point3D(1, 1, 1)), material, {BLOCK::Variation::front}),//NORTH
            Face(posBlock, HRect3D(Point3D(0, 1, 0), Point3D(1, 1, 1)), material, {BLOCK::Variation::left})//WEST
        };
    case BLOCK::Type::slab:
        return {
            Face(posBlock, HRect3D(Point3D(0, 0, 0), Point3D(1, 1, 0)), material, {BLOCK::Variation::bottom}),//BOTTOM
            Face(posBlock, HRect3D(Point3D(0, 0, 0.5L), Point3D(1, 1, 0.5L)), material, {BLOCK::Variation::top}),//TOP
            Face(posBlock, HRect3D(Point3D(0, 0, 0), Point3D(0, 1, 0.5L)), material, {BLOCK::Variation::back}),//SOUTH
            Face(posBlock, HRect3D(Point3D(0, 0, 0), Point3D(1, 0, 0.5L)), material, {BLOCK::Variation::right}),//EAST
            Face(posBlock, HRect3D(Point3D(1, 0, 0), Point3D(1, 1, 0.5L)), material, {BLOCK::Variation::front}),//NORTH
            Face(posBlock, HRect3D(Point3D(0, 1, 0), Point3D(1, 1, 0.5L)), material, {BLOCK::Variation::left})//WEST
        };
    }
    return {};
}







EntityAttribute::EntityAttribute(ENTITY::Type type) { this->type = type; }
EntityAttribute::EntityAttribute(const EntityAttribute &obj) { type = obj.type; speed = obj.speed; }
doubli EntityAttribute::getSpeed(ENTITY::Direction dir) const
{
    if(speed.contains(dir))
        return speed.value(dir);
    else
        return baseSpeed(type, dir);
}

Entity::Entity(const Pos3D &pos, ENTITY::Type type) : Object(pos)
{
    attribute.setType(type);
}


