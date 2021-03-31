#ifndef POINT3D_H
#define POINT3D_H

#include "maths3D.h"
#include <QDebug>

//#define NAN_VERIF//activer pour le debug des nan

QDebug operator<<(QDebug debug, const doubli &d);

class Point3D
{
public:
    constexpr inline Point3D() : xp(0), yp(0), zp(0) {}
    constexpr inline Point3D(const doubli &x, const doubli &y, const doubli &z)
        : xp(round(x)), yp(round(y)), zp(round(z)) {}
    // constructeur de copie 2* plus lent

    constexpr inline const doubli &x() const { return xp; }
    constexpr inline const doubli &y() const { return yp; }
    constexpr inline const doubli &z() const { return zp; }
    constexpr inline const Point3D &getPoint() const { return *this; }
    constexpr inline void setX(const doubli &x) { xp = x; }
    constexpr inline void setY(const doubli &y) { yp = y; }
    constexpr inline void setZ(const doubli &z) { zp = z; }
    constexpr inline void addX(const doubli &x) { xp += x; }
    constexpr inline void addY(const doubli &y) { yp += y; }
    constexpr inline void addZ(const doubli &z) { zp += z; }
    constexpr inline void setPoint(const Point3D &p)
    {
        xp = p.xp;
        yp = p.yp;
        zp = p.zp;
    }

    bool operator<(const Point3D &p) const;
    bool operator>(const Point3D &p) const;
    bool operator<=(const Point3D &p) const;
    bool operator>=(const Point3D &p) const;
    bool operator==(const Point3D &p) const;
    bool operator!=(const Point3D &p) const;
    bool isNull() const;
    bool isInf() const;
    bool isNan() const;

    Point3D operator+(const Point3D &p) const;
    Point3D operator-(const Point3D &p) const;
    Point3D operator*(const Point3D &p) const;
    Point3D operator*(const doubli &n) const;
    Point3D operator/(const Point3D &p) const;
    Point3D operator/(const doubli &n) const;
    // unary operator
    friend Point3D operator-(const Point3D &p);

    doubli distanceAxeZ() const;
    doubli distanceOrigine() const;
    doubli distance(const Point3D &p) const;
    doubli distanceMax(const Point3D &p) const;
    static doubli distance(const Point3D &pA, const Point3D &pB);
    static doubli distanceMax(const Point3D &pA, const Point3D &pB);

    friend QDebug operator<<(QDebug debug, const Point3D &p);
    friend Point3D qFloor(const Point3D &p);
    friend Point3D qCeil(const Point3D &p);

private:
    doubli xp, yp, zp;
};

class Point3DCancelable : public Point3D
{
public:
    constexpr inline Point3DCancelable() : valid(false) {}
    constexpr inline Point3DCancelable(const doubli &x, const doubli &y, const doubli &z)
        : Point3D(x, y, z), valid(true) {}
    constexpr inline Point3DCancelable(const Point3D &p) : Point3D(p), valid(true) {}
    constexpr inline bool isValid() const { return valid; }

private:
    const bool valid;
};

class Vec3D : public Point3D
{
public:
    constexpr inline Vec3D(const doubli &x, const doubli &y, const doubli &z) : Point3D(x, y, z) {}
    constexpr inline doubli produitScalaire(const Vec3D &vec) const
    {
        return x() * vec.x() + y() * vec.y() + z() * vec.z();
    }

    // unary operator
    friend constexpr inline Vec3D operator-(const Vec3D &p) { return {-p.x(), -p.y(), -p.z()}; }
};

#endif // POINT3D_H
