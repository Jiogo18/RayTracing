#include "Point3D.h"

QDebug operator<<(QDebug debug, const doubli &d)
{
    debug << static_cast<double>(d);
    return debug;
}

bool Point3D::operator<(const Point3D &point) const
{
    // le plus petit = la plus proche
    if (z == point.z)
        return distanceAxeZ() < point.distanceAxeZ(); // si plus proche axe Z => plus proche
    else
        return z < point.z; // si plus proche Z alors plus proche
}
bool Point3D::operator>(const Point3D &point) const
{
    // le plus grand = la plus loins
    if (z == point.z)
        return distanceAxeZ() > point.distanceAxeZ(); // si plus loins axe Z => plus loins
    else
        return z > point.z; // si plus loins Z alors plus loins
}
bool Point3D::operator==(const Point3D &point) const
{
    return (x == point.x) && (y == point.y) && (z == point.z);
}

bool Point3D::operator!=(const Point3D &point) const
{
    return (x != point.x) || (y != point.y) || (z != point.z);
}

bool Point3D::operator<=(const Point3D &point) const
{
    return operator<(point) || operator==(point);
}

bool Point3D::operator>=(const Point3D &point) const
{
    return operator>(point) || operator==(point);
}

Point3D Point3D::operator+(const Point3D &point) const
{
    return Point3D(x + point.x, y + point.y, z + point.z);
}
Point3D Point3D::operator-(const Point3D &point) const
{
    return Point3D(x - point.x, y - point.y, z - point.z);
}
Point3D Point3D::operator*(const Point3D &point) const
{
    return Point3D(x * point.x, y * point.y, z * point.z);
}
Point3D Point3D::operator*(doubli n) const { return Point3D(x * n, y * n, z * n); }
Point3D Point3D::operator/(const Point3D &point) const
{
    return Point3D(x / point.x, y / point.y, z / point.z);
}
Point3D Point3D::operator/(doubli n) const { return Point3D(x / n, y / n, z / n); }

doubli Point3D::distance(const Point3D &pA, const Point3D &pB)
{
    return sqrt(sqr(pB.x - pA.x) + sqr(pB.y - pA.y) + sqr(pB.z - pA.z));
}
doubli Point3D::distanceMax(const Point3D &pA, const Point3D &pB)
{
    return max(abs(pB.x - pA.x), abs(pB.y - pA.y), abs(pB.z - pA.z));
    // on prend le plus grand (ça forme un carré)
}

doubli Point3D::distance(const Point3D &pA) const
{
    return sqrt((x - pA.x) * (x - pA.x) + (y - pA.y) * (y - pA.y) + (z - pA.z) * (z - pA.z));
}

doubli Point3D::distanceMax(const Point3D &pA) const
{
    return max(abs(x - pA.x), abs(y - pA.y), abs(z - pA.z));
    // on prend le plus grand (ça forme un carré)
}

QDebug operator<<(QDebug debug, const Point3D &point)
{
    debug << "Point3D(" << point.x << "," << point.y << "," << point.z << ")";
    return debug;
}
Point3D qFloor(const Point3D &point)
{
    return Point3D(std::floor(point.x), std::floor(point.y), std::floor(point.z));
}
Point3D qCeil(const Point3D &point)
{
    return Point3D(std::ceil(point.x), std::ceil(point.y), std::ceil(point.z));
}
