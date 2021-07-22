#ifndef POINT3D_H
#define POINT3D_H

#include "maths3D.h"
#include <QDebug>

//#define NAN_VERIF//activer pour le debug des nan
using namespace maths3D;

QDebug operator<<(QDebug debug, const doubli &d);

/*****************************************************************************
  Point3D
 *****************************************************************************/

class Point3D
{
public:
    constexpr inline Point3D() : xp(0), yp(0), zp(0) {}
    constexpr inline Point3D(const doubli &x, const doubli &y, const doubli &z)
        : xp(roundDoubli(x)), yp(roundDoubli(y)), zp(roundDoubli(z)) {}
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
    constexpr inline void setPoint(const Point3D &p);

    bool operator<(const Point3D &p) const;
    bool operator>(const Point3D &p) const;
    bool operator<=(const Point3D &p) const;
    bool operator>=(const Point3D &p) const;
    bool operator==(const Point3D &p) const;
    bool operator!=(const Point3D &p) const;
    bool isNull() const;
    bool isInf() const;
    bool isNan() const;

    constexpr inline Point3D operator+(const Point3D &p) const;
    constexpr inline Point3D operator-(const Point3D &p) const;
    constexpr inline Point3D operator*(const Point3D &p) const;
    constexpr inline Point3D operator*(const doubli &n) const;
    constexpr inline Point3D operator/(const Point3D &p) const;
    constexpr inline Point3D operator/(const doubli &n) const;
    friend constexpr inline Point3D operator-(const Point3D &p);

    doubli distanceAxeZ() const;
    doubli distanceOrigine() const;
    doubli distance(const Point3D &p) const;
    doubli distanceMax(const Point3D &p) const;
    static doubli distance(const Point3D &pA, const Point3D &pB);
    static doubli distanceMax(const Point3D &pA, const Point3D &pB);

    friend QDebug operator<<(QDebug debug, const Point3D &p);
    friend Point3D qFloor(const Point3D &p);
    friend Point3D qCeil(const Point3D &p);
    friend Point3D qRound(const Point3D &p);

private:
    doubli xp, yp, zp;

    friend class Vec3D;
    friend constexpr inline class Vec3D operator-(const class Vec3D &p);
};

/*****************************************************************************
  Point3D constexpr & inline (big) functions
 *****************************************************************************/

constexpr inline void Point3D::setPoint(const Point3D &p)
{
    xp = p.xp;
    yp = p.yp;
    zp = p.zp;
}

constexpr inline Point3D Point3D::operator+(const Point3D &p) const { return Point3D{xp + p.xp, yp + p.yp, zp + p.zp}; }
constexpr inline Point3D Point3D::operator-(const Point3D &p) const { return Point3D{xp - p.xp, yp - p.yp, zp - p.zp}; }
constexpr inline Point3D Point3D::operator*(const Point3D &p) const { return Point3D{xp * p.xp, yp * p.yp, zp * p.zp}; }
constexpr inline Point3D Point3D::operator*(const doubli &n) const { return Point3D{xp * n, yp * n, zp * n}; }
constexpr inline Point3D Point3D::operator/(const Point3D &p) const { return Point3D{xp / p.xp, yp / p.yp, zp / p.zp}; }
constexpr inline Point3D Point3D::operator/(const doubli &n) const { return Point3D{xp / n, yp / n, zp / n}; }
constexpr inline Point3D operator-(const Point3D &p) { return Point3D{-p.xp, -p.yp, -p.zp}; }

inline doubli Point3D::distanceAxeZ() const { return distanceLoxodromique_2(xp, yp); }
inline doubli Point3D::distanceOrigine() const { return distanceLoxodromique_3(xp, yp, zp); }
inline doubli Point3D::distance(const Point3D &p) const { return distanceLoxodromique_3(xp - p.xp, yp - p.yp, zp - p.zp); }
inline doubli Point3D::distanceMax(const Point3D &p) const { return MAX_ABS_3(xp - p.xp, yp - p.yp, zp - p.zp); }
inline doubli Point3D::distance(const Point3D &pA, const Point3D &pB) { return distanceLoxodromique_3(pB.xp - pA.xp, pB.yp - pA.yp, pB.zp - pA.zp); }
inline doubli Point3D::distanceMax(const Point3D &pA, const Point3D &pB) { return MAX_ABS_3(pB.xp - pA.xp, pB.yp - pA.yp, pB.zp - pA.zp); }

/*****************************************************************************
  Point3DCancelable
  An extension of Point3D to invalidate a point without affecting performance
 *****************************************************************************/

class Point3DCancelable : public Point3D
{
public:
    constexpr inline Point3DCancelable() : valid(false) {}
    constexpr inline Point3DCancelable(const doubli &x, const doubli &y, const doubli &z)
        : Point3D(x, y, z), valid(true) {}
    constexpr inline Point3DCancelable(const Point3D &p) : Point3D(p), valid(true) {}
    constexpr inline const bool &isValid() const { return valid; }

private:
    const bool valid;
};

/*****************************************************************************
  Vec3D
  An extension of Point3D for Vectors
 *****************************************************************************/

class Vec3D : public Point3D
{
public:
    constexpr inline Vec3D(const doubli &x, const doubli &y, const doubli &z) : Point3D(x, y, z) {}
    constexpr inline Vec3D(const Point3D &p) : Point3D(p) {}
    constexpr inline doubli produitScalaire(const Vec3D &vec) const;
    constexpr inline doubli produitScalaire(const doubli &x, const doubli &y, const doubli &z) const;
};

/*****************************************************************************
  Vec3D constexpr & inline (big) functions
 *****************************************************************************/

constexpr inline doubli Vec3D::produitScalaire(const Vec3D &vec) const { return xp * vec.xp + yp * vec.yp + zp * vec.zp; }
constexpr inline doubli Vec3D::produitScalaire(const doubli &x, const doubli &y, const doubli &z) const { return xp * x + yp * y + zp * z; }

constexpr inline Vec3D operator-(const Vec3D &p) { return Vec3D{-p.xp, -p.yp, -p.zp}; }

#endif // POINT3D_H
