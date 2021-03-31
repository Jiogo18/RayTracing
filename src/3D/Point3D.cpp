#include "Point3D.h"

QDebug operator<<(QDebug debug, const doubli &d)
{
    debug << static_cast<double>(d);
    return debug;
}

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
bool Point3D::isNan() const { return isnanf(xp + yp + zp); }

Point3D Point3D::operator+(const Point3D &p) const { return {xp + p.xp, yp + p.yp, zp + p.zp}; }
Point3D Point3D::operator-(const Point3D &p) const { return {xp - p.xp, yp - p.yp, zp - p.zp}; }
Point3D Point3D::operator*(const Point3D &p) const { return {xp * p.xp, yp * p.yp, zp * p.zp}; }
Point3D Point3D::operator*(const doubli &n) const { return {xp * n, yp * n, zp * n}; }
Point3D Point3D::operator/(const Point3D &p) const { return {xp / p.xp, yp / p.yp, zp / p.zp}; }
Point3D Point3D::operator/(const doubli &n) const { return {xp / n, yp / n, zp / n}; }
Point3D operator-(const Point3D &p) { return {-p.xp, -p.yp, -p.zp}; }

doubli Point3D::distanceAxeZ() const { return distanceLoxodromique_2(xp, yp); }
doubli Point3D::distanceOrigine() const { return distanceLoxodromique_3(xp, yp, zp); }
doubli Point3D::distance(const Point3D &p) const
{
    return distanceLoxodromique_3(xp - p.xp, yp - p.yp, zp - p.zp);
}
doubli Point3D::distanceMax(const Point3D &p) const
{
    return MAX_ABS_3(xp - p.xp, yp - p.yp, zp - p.zp);
}
doubli Point3D::distance(const Point3D &pA, const Point3D &pB)
{
    return distanceLoxodromique_3(pB.xp - pA.xp, pB.yp - pA.yp, pB.zp - pA.zp);
}
doubli Point3D::distanceMax(const Point3D &pA, const Point3D &pB)
{
    return MAX_ABS_3(pB.xp - pA.xp, pB.yp - pA.yp, pB.zp - pA.zp);
}

QDebug operator<<(QDebug debug, const Point3D &p)
{
    debug.nospace() << "Point3D(" << p.xp << ", " << p.yp << ", " << p.zp << ')';
    return debug;
}
Point3D qFloor(const Point3D &p)
{
    return {std::floor(p.xp), std::floor(p.yp), std::floor(p.zp)};
}
Point3D qCeil(const Point3D &p)
{
    return {std::ceil(p.xp), std::ceil(p.yp), std::ceil(p.zp)};
}
