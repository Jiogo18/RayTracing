#include "Pos3D.h"

bool Rot3D::operator==(const Rot3D &rot) const { return rXp == rot.rXp && rZp == rot.rZp; }
bool Rot3D::operator!=(const Rot3D &rot) const { return rXp != rot.rXp || rZp != rot.rZp; }

bool Rot3D::isNan() const { return isnan(rXp + rZp); }

std::ostream &operator<<(std::ostream &os, const Rot3D &rot)
{
    os << "Rot3D(" << rot.rXp << ", " << rot.rZp << ")";
    return os;
}

Pos3D Pos3D::fromDegree(const doubli &x, const doubli &y, const doubli &z, const radian &rX, const radian &rZ)
{
    return Pos3D{x, y, z, degreesToRadians(rX), degreesToRadians(rZ)};
}

void Pos3D::moveWithRot(const doubli &speed, const radian &rot)
{
    // trigo, on connait l'hypothénuse (speed) et l'angle et on veut coté a ou b => cos ou sin
    addX(cosTaylorMin(rX() + rot) * speed);
    addY(sinTaylorMin(rX() + rot) * speed);
}

Point3D Pos3D::pointFromRot(const doubli &d, const radian &rX, const radian &rZ)
{
    return Point3D{cosTaylorMin(rZ) * cosTaylorMin(rX), cosTaylorMin(rZ) * sinTaylorMin(rX), sinTaylorMin(rZ) * d} * d;
}

Pos3D Pos3D::getChildRot(const radian &rXRelatif, const radian &rZRelatif) const
{
    // changeRef de childNext
    const doubli crZR = cosTaylorMin(rZRelatif),
                 crZ = cosTaylorMin(rZ()), crX = cosTaylorMin(rX()),
                 srZ = sinTaylorMin(rZ()), srX = sinTaylorMin(rX());
    const doubli x1 = crZR * cosTaylorMin(rXRelatif), y1 = crZR * sinTaylorMin(rXRelatif), z1 = sinTaylorMin(rZRelatif); // z1==srZR
    const doubli x2 = x1 * crZ - z1 * srZ, z2 = x1 * srZ + z1 * crZ;                                                     // y2==y1
    const doubli x3 = x2 * crX - y1 * srX, y3 = x2 * srX + y1 * crX;                                                     // z3==z2

    radian rX = atan(y3 / x3);
    if (x3 < 0) {   // sinon on ne voit que tout ce qui est en x3>=0 (rotation de 180° du reste)
        rX += M_PI; // atan retourne -89.5 alors qu'il faut 90.5 (mais ça reste la meme chose)
        // pas besoin de différentier y car on est à k360° de diff
    }
    return Pos3D{static_cast<Point3D>(*this), rX, (radian)asin(z2)};
}

Point3D Pos3D::changeRef(const Point3D &point, const doubli &srX, const doubli &srZ) const
{
    const doubli x = point.x() + this->x(),
                 z = point.z() + this->z(),
                 crZ = sqrt(1 - srZ * srZ),
                 crX = sqrt(1 - srX * srX),
                 x2 = x * crZ - z * srZ;
    return Point3D{x2 * crX - (point.y() + this->y()) * srX,
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

bool Pos3D::isNan() const { return Point3D::isNan() || Rot3D::isNan(); }

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
    return Pos3D{p1, rX, rZ};
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
    return Pos3D{0, 0, 0, rX, rZ};
}

std::ostream &operator<<(std::ostream &os, const Pos3D &pos)
{
    os << "Pos3D(" << pos.x() << ", " << pos.y() << ", " << pos.z()
       << ";" << pos.rX() << ", " << pos.rZ() << ")";
    return os;
}
