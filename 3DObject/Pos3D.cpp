#include "Pos3D.h"

Rot3D Rot3D::fromVector(const Point3D &v)
{
    //https://fr.wikipedia.org/wiki/Coordonn%C3%A9es_polaires
    //https://wikimedia.org/api/rest_v1/media/math/render/svg/d4a6b8b7e93b05abc00d7789635a762b495d0d67
#ifdef NAN_VERIF
    const radian rX = 2 * atan(y / (x + sqrt(x * x + y * y))), rZ = asin(z / vect.distanceOrigine());
    if (isnanf(rX) || isnanf(rZ)) {
        qDebug() << "Rot3D::fromVector is nan:" << rX << rZ << vect;
        exit(-1);
    }
#else
    return {static_cast<radian>(atan2(v.y(), v.x())), static_cast<radian>(asin(v.z() / v.distanceOrigine()))};
    //return { 2 * atan(v.y() / (v.x() + v.distanceAxeZ())), asin(v.z() / v.distanceOrigine()) };
#endif
}

Vec3D Rot3D::toVector() const { return {cos(rXp) * cos(rZp), sin(rXp) * cos(rZp), sin(rZp)}; }

bool Rot3D::operator==(const Rot3D &rot) const { return rXp == rot.rXp && rZp == rot.rZp; }
bool Rot3D::operator!=(const Rot3D &rot) const { return rXp != rot.rXp || rZp != rot.rZp; }

bool Rot3D::isNan() const { return isnanf(rXp + rZp); }

QDebug operator<<(QDebug debug, const Rot3D &rot)
{
    debug.nospace() << "Rot3D(" << rot.rXp << ", " << rot.rZp << ")";
    return debug;
}

Pos3D Pos3D::fromDegree(const doubli &x, const doubli &y, const doubli &z, const radian &rX, const radian &rZ)
{
    return {x, y, z, degreesToRadians(rX), degreesToRadians(rZ)};
}

void Pos3D::moveWithRot(const doubli &speed, const radian &rot)
{
    // trigo, on connait l'hypothénuse (speed) et l'angle et on veut coté a ou b => cos ou sin
    addX(cos(rX() + rot) * speed);
    addY(sin(rX() + rot) * speed);
}

Point3D Pos3D::pointFromRot(const doubli &d, const radian &rX, const radian &rZ)
{
    return Point3D(cos(rZ) * cos(rX), cos(rZ) * sin(rX), sin(rZ) * d) * d;
}

Point3D Pos3D::getNextPoint() const
{
    return {cos(rZ()) * cos(rX()) + x(), cos(rZ()) * sin(rX()) + y(), sin(rZ()) + z()};
    // return getPoint() + toVector();//équivalent
}

Pos3D Pos3D::getChildRot(const radian &rXRelatif, const radian &rZRelatif) const
{
    // changeRef de childNext
    const doubli crZR = cos(rZRelatif),
                 crZ = cos(rZ()), crX = cos(rX()),
                 srZ = sin(rZ()), srX = sin(rX());
    const doubli x1 = crZR * cos(rXRelatif), y1 = crZR * sin(rXRelatif), z1 = sin(rZRelatif); // z1==srZR
    const doubli x2 = x1 * crZ - z1 * srZ, z2 = x1 * srZ + z1 * crZ;                          // y2==y1
    const doubli x3 = x2 * crX - y1 * srX, y3 = x2 * srX + y1 * crX;                          // z3==z2

    radian rX = atan(y3 / x3);
    if (x3 < 0) {   // sinon on ne voit que tout ce qui est en x3>=0 (rotation de 180° du reste)
        rX += M_PI; // atan retourne -89.5 alors qu'il faut 90.5 (mais ça reste la meme chose)
        // pas besoin de différentier y car on est à k360° de diff
    }
    return {static_cast<Point3D>(*this), rX, asin(z2)};
}

Point3D Pos3D::changeRef(const Point3D &point, const doubli &srX, const doubli &srZ) const
{
    const doubli x = point.x() + this->x(),
                 z = point.z() + this->z(),
                 crZ = sqrt(1 - srZ * srZ),
                 crX = sqrt(1 - srX * srX),
                 x2 = x * crZ - z * srZ;
    return {x2 * crX - (point.y() + this->y()) * srX,
            x2 * srX + (point.y() + this->y()) * crX,
            x * srZ + z * crZ};
    // condensé des équations de rotation dans Oxz puis dans Oxy
    // x * sR + y * cR;
    // x * cR - y * sR;
}

bool Pos3D::operator==(const Pos3D &pos) const
{
    return Point3D::operator==(pos) && Rot3D::operator==(pos);
}

bool Pos3D::operator!=(const Pos3D &pos) const
{
    return Point3D::operator!=(pos) || Rot3D::operator!=(pos);
}

bool Pos3D::isNan() const { return Point3D::isNull(); }

Pos3D Pos3D::getRotAsVect(const Point3D &p1, const Point3D &p2)
{
    radian rZ = asin((p2.z() - p1.z()) / p1.distance(p2)); // formule getNextPos z inversée
    radian rX = atan((p2.y() - p1.y()) / (p2.x() - p1.x()));
    if (p2.x() - p1.x() < 0) {
        if (p2.y() - p1.y() >= 0)
            rX += M_PI; // atan retourne -89.5 alors qu'il faut 90.5 (mais ça reste la meme chose)
        else
            rX -= M_PI;
    }
    return {p1, rX, rZ};
}

Pos3D Pos3D::getRotAsPoint(const Point3D &p)
{
    // opti mais == getRotAsVect(Point3D(0,0,0),p)
    radian rZ = asin(p.z() / p.distanceOrigine()); // formule getNextPos z inversée
    radian rX = atan(p.y() / p.x());
    if (p.x() < 0) {
        if (p.y() >= 0)
            rX += M_PI; // atan retourne -89.5 alors qu'il faut 90.5 (mais ça reste la meme chose)
        else
            rX -= M_PI;
    }
    return {0, 0, 0, rX, rZ};
}

QDebug operator<<(QDebug debug, const Pos3D &pos)
{
    debug.nospace() << "Pos3D(" << pos.x() << ", " << pos.y() << ", " << pos.z()
                    << ";" << pos.rX() << ", " << pos.rZ() << ")";
    return debug;
}
