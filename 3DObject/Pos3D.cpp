#include "Pos3D.h"



Rot3D Rot3D::fromVector(const Point3D& vect)
{
    //https://fr.wikipedia.org/wiki/Coordonn%C3%A9es_polaires
    //https://wikimedia.org/api/rest_v1/media/math/render/svg/d4a6b8b7e93b05abc00d7789635a762b495d0d67
    doubli x = vect.getX(), y = vect.getY(), z = vect.getZ();
    doubli rX = 2 * atan(y / (x + sqrt(x * x + y * y)));
    doubli rZ = asin(z / sqrt(x * x + y * y + z * z));
#ifdef NAN_VERIF
    if (isnanf(rX) || isnanf(rZ)) {
        qDebug() << "Rot3D::fromVector is nan:" << rX << rZ << vect;
        exit(-1);
    }
#endif
    return Rot3D(rX, rZ);
}

Vec3D Rot3D::toVector() const
{
    return Vec3D(cos(rX) * cos(rZ), sin(rX) * cos(rZ), sin(rZ));
}

Vec3D Rot3D::toVectorU() const
{
    doubli x = cos(rX) * cos(rZ), y = sin(rX) * cos(rZ), z = sin(rZ);
    doubli d = sqrt(x * x + y * y + z * z);
    return Vec3D(x / d, y / d, z / d);
}

QDebug operator <<(QDebug debug, const Rot3D& rot)
{
    debug << "Rot3D(" << rot.rX << "," << rot.rZ << ")";
    return debug;
}




Pos3D::Pos3D() : Point3D(), Rot3D() {}
Pos3D::Pos3D(doubli x, doubli y, doubli z, radian rX, radian rZ) : Point3D(x, y, z), Rot3D(rX, rZ) {}
Pos3D::Pos3D(const Point3D& point, radian rX, radian rZ) : Point3D(point), Rot3D(rX, rZ) {}
Pos3D::Pos3D(const Point3D& point, const Rot3D& rot) : Point3D(point), Rot3D(rot) {}
Pos3D::Pos3D(const Pos3D& pos) : Point3D(pos), Rot3D(pos) {}

Pos3D* Pos3D::operator=(const Pos3D& pos)
{
    Point3D::operator=(pos);
    Rot3D::operator=(pos);
    return this;
}

Pos3D Pos3D::fromDegree(doubli x, doubli y, doubli z, radian rX, radian rZ)
{
    return Pos3D(x, y, z, degreesToRadians(rX), degreesToRadians(rZ));
}

void Pos3D::moveWithRot(doubli speed, radian rot)
{
    //trigo, on connait l'hypothénuse (speed) et l'angle et on veut coté a ou b => cos ou sin
    addX(cos(rX + rot) * speed);
    addY(sin(rX + rot) * speed);
}

Point3D Pos3D::pointFromRot(doubli d, radian rX, radian rZ)
{
    return Point3D(cos(rZ) * cos(rX),
        cos(rZ) * sin(rX),
        sin(rZ) * d) * d;
}

Point3D Pos3D::getNextPoint() const
{
    return Point3D(cos(rZ) * cos(rX) + x, cos(rZ) * sin(rX) + y, sin(rZ) + z);
    //return getPoint() + toVector();//équivalent
}

Pos3D Pos3D::getChildRot(radian rXRelatif, radian rZRelatif) const
{
    //changeRef de childNext
    doubli crZR = cos(rZRelatif),
        crZ = cos(rZ), crX = cos(rX),
        srZ = sin(rZ), srX = sin(rX);
    doubli x1 = crZR * cos(rXRelatif), y1 = crZR * sin(rXRelatif), z1 = sin(rZRelatif);//z1==srZR
    doubli x2 = x1 * crZ - z1 * srZ, z2 = x1 * srZ + z1 * crZ;//y2==y1
    doubli x3 = x2 * crX - y1 * srX, y3 = x2 * srX + y1 * crX;//z3==z2
    radian rX = atan(y3 / x3);
    if (x3 < 0) {//sinon on ne voit que tout ce qui est en x3>=0 (rotation de 180° du reste)
        rX += M_PI;//atan retourne -89.5 alors qu'il faut 90.5 (mais ça reste la meme chose)
        //pas besoin de différentier y car on est à k360° de diff
    }
    return Pos3D(static_cast<Point3D>(*this), rX, asin(z2));
}

Point3D Pos3D::changeRef(const Point3D point, doubli srX, doubli srZ) const
{
    doubli x = point.getX() + this->x,
        z = point.getZ() + this->z,
        crZ = sqrt(1 - srZ * srZ),
        crX = sqrt(1 - srX * srX),
        x2 = x * crZ - z * srZ;
    return Point3D(x2 * crX - (point.getY() + this->y) * srX,
        x2 * srX + (point.getY() + this->y) * crX,
        x * srZ + z * crZ);
    // condensé des équations de rotation dans Oxz puis dans Oxy
    // x * sR + y * cR;
    // x * cR - y * sR;
}

bool Pos3D::operator ==(const Pos3D& pos) const
{
    return Point3D::operator ==(pos) && Rot3D::operator ==(pos);
}

bool Pos3D::operator !=(const Pos3D& pos) const
{
    return Point3D::operator !=(pos) || Rot3D::operator !=(pos);
}

Pos3D Pos3D::getRotAsVect(const Point3D& p1, const Point3D& p2)
{
    radian rZ = asin((p2.getZ() - p1.getZ()) / Point3D::distance(p1, p2));//formule getNextPos z inversée
    radian rX = atan((p2.getY() - p1.getY()) / (p2.getX() - p1.getX()));
    if (p2.getX() - p1.getX() < 0) {
        if (p2.getY() - p1.getY() >= 0)
            rX += M_PI;//atan retourne -89.5 alors qu'il faut 90.5 (mais ça reste la meme chose)
        else
            rX -= M_PI;
    }
    return Pos3D(p1, rX, rZ);
}

Pos3D Pos3D::getRotAsPoint(const Point3D& p)
{
    //opti mais == getRotAsVect(Point3D(0,0,0),p)
    doubli d = Point3D::distance(Point3D(0, 0, 0), p);
    radian rZ = asin((p.getZ()) / d);//formule getNextPos z inversée
    radian rX = atan((p.getY()) / (p.getX()));
    if (p.getX() < 0) {
        if (p.getY() >= 0)
            rX += M_PI;//atan retourne -89.5 alors qu'il faut 90.5 (mais ça reste la meme chose)
        else
            rX -= M_PI;
    }
    return Pos3D(0, 0, 0, rX, rZ);
}


QDebug operator <<(QDebug debug, const Pos3D& pos)
{
    debug << "Pos3D(" << pos.x << "," << pos.y << "," << pos.z
        << ";" << pos.rX << "," << pos.rZ << ")";
    return debug;
}
