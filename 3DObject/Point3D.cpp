#include "Point3D.h"



bool isNull(const doubli d) { return -0.000001 < d && d < 0.000001; }
doubli roundNull(const doubli d) { return isNull(d) ? 0 : d; }
doubli signOf(const doubli d) { return d < 0 ? -1 : 1; }
doubli sqr(const doubli d) { return d * d; }
radian degreesToRadians(const doubli deg) { return fmodl(deg, 360) * doubli(M_PI) / 180; }
doubli radiansToDegrees(const radian rad) { return rad * 180 / doubli(M_PI); }
doubli sqrt(const doubli d) { return std::sqrt(d); }
doubli cos(const radian deg) { return std::cos(deg); }
doubli sin(const radian deg) { return std::sin(deg); }
doubli tan(const radian deg) { return std::tan(deg); }
radian acos(const doubli d) { return std::acos(d); }
radian asin(const doubli d) { return std::asin(d); }
radian atan(const doubli d) { return std::atan(d); }
doubli qIsInf(const doubli d) { return qIsInf(static_cast<double>(d)); }
doubli max(const doubli a, const doubli b) { return a > b ? a : b; }
doubli max(const doubli a, const doubli b, const doubli c) { return a > b ? (a > c ? a : c) : (b > c ? b : c); }
doubli min(const doubli a, const doubli b) { return a < b ? a : b; }
doubli min(const doubli a, const doubli b, const doubli c) { return a < b ? (a < c ? a : c) : (b < c ? b : c); }
bool isNull(const radian d) { return -0.000001 < d && d < 0.000001; }
radian mod(const radian n, const radian d) { return (n - floor(n / d) * d); }
radian modRad(radian d) { d = (d - floor(d / (2 * M_PI)) * 2 * M_PI); return d > M_PI ? d - 2 * M_PI : d; }
radian roundNull(radian d) { d = modRad(d); return isNull(d) ? 0 : d; }
float signOf(radian d) { return modRad(d) < 0 ? -1 : 1; }
QDebug operator <<(QDebug debug, const doubli d) { debug << static_cast<double>(d); return debug; }




bool Point3D::operator <(const Point3D& point) const
{
    //le plus petit = la plus proche
    if (z == point.z)
        return distanceAxeZ() < point.distanceAxeZ();//si plus proche axe Z => plus proche
    else
        return z < point.z;//si plus proche Z alors plus proche
}
bool Point3D::operator >(const Point3D& point) const
{
    //le plus grand = la plus loins
    if (z == point.z)
        return distanceAxeZ() > point.distanceAxeZ();//si plus loins axe Z => plus loins
    else
        return z > point.z;//si plus loins Z alors plus loins
}
bool Point3D::operator ==(const Point3D& point) const
{
    return (x == point.x) && (y == point.y) && (z == point.z);
}

bool Point3D::operator !=(const Point3D& point) const
{
    return (x != point.x) || (y != point.y) || (z != point.z);
}

bool Point3D::operator <=(const Point3D& point) const
{
    return operator <(point) || operator ==(point);
}

bool Point3D::operator >=(const Point3D& point) const
{
    return operator >(point) || operator ==(point);
}

Point3D Point3D::operator +(const Point3D& point) const
{
    return Point3D(x + point.x,
        y + point.y,
        z + point.z);
}
Point3D Point3D::operator -(const Point3D& point) const
{
    return Point3D(x - point.x,
        y - point.y,
        z - point.z);
}
Point3D Point3D::operator *(const Point3D& point) const
{
    return Point3D(x * point.x,
        y * point.y,
        z * point.z);
}
Point3D Point3D::operator *(doubli n) const { return Point3D(x * n, y * n, z * n); }
Point3D Point3D::operator /(const Point3D& point) const
{
    return Point3D(x / point.x,
        y / point.y,
        z / point.z);
}
Point3D Point3D::operator /(doubli n) const { return Point3D(x / n, y / n, z / n); }

doubli Point3D::distance(const Point3D& pA, const Point3D& pB)
{
    return sqrt(sqr(pB.x - pA.x) +
        sqr(pB.y - pA.y) +
        sqr(pB.z - pA.z));
}
doubli Point3D::distanceMax(const Point3D& pA, const Point3D& pB)
{
    return max(abs(pB.x - pA.x), abs(pB.y - pA.y), abs(pB.z - pA.z));
    //on prend le plus grand (ça forme un carré)
}

doubli Point3D::distance(const Point3D& pA) const
{
    return sqrt((x - pA.x) * (x - pA.x) + (y - pA.y) * (y - pA.y) + (z - pA.z) * (z - pA.z));
}

doubli Point3D::distanceMax(const Point3D& pA) const
{
    return max(abs(x - pA.x), abs(y - pA.y), abs(z - pA.z));
    //on prend le plus grand (ça forme un carré)
}

QDebug operator <<(QDebug debug, const Point3D& point)
{
    debug << "Point3D(" << point.x << "," << point.y << "," << point.z << ")";
    return debug;
}
Point3D qFloor(const Point3D& point)
{
    return Point3D(std::floor(point.x), std::floor(point.y), std::floor(point.z));
}
Point3D qCeil(const Point3D& point)
{
    return Point3D(std::ceil(point.x), std::ceil(point.y), std::ceil(point.z));
}

