#include "Point3D.h"

bool Point3D::operator<(const Point3D &p) const
{
    // le plus petit = la plus proche
    if (zp == p.zp)
        return distanceAxeZ() < p.distanceAxeZ(); // si plus proche axe Z => plus proche
    else
        return zp < p.zp; // si plus proche Z alors plus proche
}

bool Point3D::operator>(const Point3D &p) const
{
    // le plus grand = la plus loins
    if (zp == p.zp)
        return distanceAxeZ() > p.distanceAxeZ(); // si plus loins axe Z => plus loins
    else
        return zp > p.zp; // si plus loins Z alors plus loins
}

bool Point3D::operator<=(const Point3D &p) const
{
    if (zp == p.zp)
        return distanceAxeZ() <= p.distanceAxeZ(); //si plus proche axe Z => plus proche
    else
        return zp < p.zp; //si plus proche Z alors plus proche
}

bool Point3D::operator>=(const Point3D &p) const
{
    if (zp == p.zp)
        return distanceAxeZ() >= p.distanceAxeZ(); //si plus loins axe Z => plus loins
    else
        return zp > p.zp; //si plus loins Z alors plus loins
}

bool Point3D::operator==(const Point3D &p) const { return (xp == p.xp) && (yp == p.yp) && (zp == p.zp); }
bool Point3D::operator!=(const Point3D &p) const { return (xp != p.xp) || (yp != p.yp) || (zp != p.zp); }

bool Point3D::isNull() const { return xp == 0 && yp == 0 && zp == 0; }
bool Point3D::isInf() const { return isinf(xp) || isinf(yp) || isinf(zp); }
bool Point3D::isNan() const { return isnan(xp + yp + zp); }

std::ostream &operator<<(std::ostream &os, const Point3D &p)
{
    os << "Point3D(" << p.xp << ", " << p.yp << ", " << p.zp << ')';
    return os;
}
Point3D qFloor(const Point3D &p)
{
    return Point3D{std::floor(p.xp), std::floor(p.yp), std::floor(p.zp)};
}
Point3D qCeil(const Point3D &p)
{
    return Point3D{std::ceil(p.xp), std::ceil(p.yp), std::ceil(p.zp)};
}
Point3D qRound(const Point3D &p)
{
    return Point3D{std::round(p.xp), std::round(p.yp), std::round(p.zp)};
}
