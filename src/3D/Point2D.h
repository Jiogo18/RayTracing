#pragma once

#include "maths3D.h"
#include <iostream>

using namespace maths3D;

/*****************************************************************************
  Point2D
 *****************************************************************************/

class Point2D
{
public:
    constexpr inline Point2D() : xp(0), yp(0) {}
    // constexpr inline Point2D(const doubli &x, const doubli &y)
    //     : xp(roundDoubli(x)), yp(roundDoubli(y)) {}
    // constructeur de copie 2* plus lent
    constexpr inline Point2D(const doubli &x, const doubli &y) : xp(x), yp(y) {}

    constexpr inline const doubli &x() const { return xp; }
    constexpr inline const doubli &y() const { return yp; }
    // constexpr inline const Point2D &getPoint() const { return *this; }
    // constexpr inline void setX(const doubli &x) { xp = x; }
    // constexpr inline void setY(const doubli &y) { yp = y; }
    // constexpr inline void addX(const doubli &x) { xp += x; }
    // constexpr inline void addY(const doubli &y) { yp += y; }
    // constexpr inline void setPoint(const Point2D &p);

    // bool operator<(const Point2D &p) const;
    // bool operator>(const Point2D &p) const;
    // bool operator<=(const Point2D &p) const;
    // bool operator>=(const Point2D &p) const;
    // bool operator==(const Point2D &p) const;
    // bool operator!=(const Point2D &p) const;
    // bool isNull() const;
    // bool isInf() const;
    // bool isNan() const;

    // constexpr inline Point2D operator+(const Point2D &p) const;
    Point2D operator-(const Point2D &p) const;
    // constexpr inline Point2D operator*(const Point2D &p) const;
    // constexpr inline Point2D operator*(const doubli &n) const;
    // constexpr inline Point2D operator/(const Point2D &p) const;
    // constexpr inline Point2D operator/(const doubli &n) const;
    // friend constexpr inline Point2D operator-(const Point2D &p);

    // doubli distanceOrigine() const;
    // doubli distance(const Point2D &p) const;
    // doubli distanceMax(const Point2D &p) const;
    // static doubli distance(const Point2D &pA, const Point2D &pB);
    // static doubli distanceMax(const Point2D &pA, const Point2D &pB);

    // friend Point2D qFloor(const Point2D &p);
    // friend Point2D qCeil(const Point2D &p);
    // friend Point2D qRound(const Point2D &p);

    friend std::ostream &operator<<(std::ostream &os, const Point2D &p);

private:
    doubli xp, yp;
};
