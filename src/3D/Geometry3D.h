#ifndef GEOMETRY3D_H
#define GEOMETRY3D_H

#include <QPointF>
#include "Pos3D.h"

/*****************************************************************************
  Size3D : delta from a point to another
    delta X, delta Y, delta Z
    (or width, depth, height)
 *****************************************************************************/

class Size3D
{
public:
    constexpr inline Size3D() : dXp(0), dYp(0), dZp(0) {}
    constexpr inline Size3D(const doubli &dX, const doubli &dY, const doubli &dZ) : dXp(dX), dYp(dY), dZp(dZ) {}
    constexpr inline Size3D(const Size3D &s) : dXp(s.dXp), dYp(s.dYp), dZp(s.dZp) {}
    constexpr inline Size3D(const Point3D &pA, const Point3D &pB)
        : dXp(pB.x() - pA.x()), dYp(pB.y() - pA.y()), dZp(pB.z() - pA.z()) {}

    constexpr inline const doubli &dX() const { return dXp; }
    constexpr inline const doubli &dY() const { return dYp; }
    constexpr inline const doubli &dZ() const { return dZp; }
    constexpr inline const Size3D &getSize() const { return *this; }
    constexpr inline Point3D toPoint() const { return Point3D(dXp, dYp, dZp); } // relative to (0,0,0)

    constexpr inline void setDX(const doubli &dX) { dXp = dX; }
    constexpr inline void setDY(const doubli &dY) { dYp = dY; }
    constexpr inline void setDZ(const doubli &dZ) { dZp = dZ; }

    constexpr inline Size3D operator*(const doubli &scale) const { return Size3D(dXp * scale, dYp * scale, dZp * scale); }
    constexpr inline Size3D operator/(const doubli &scale) const { return Size3D(dXp / scale, dYp / scale, dZp / scale); }

    friend constexpr inline Point3D operator-(const Point3D &p, const Size3D &s) { return Point3D(p.x() - s.dXp, p.y() - s.dYp, p.z() - s.dZp); }
    friend constexpr inline Point3D operator+(const Point3D &p, const Size3D &s) { return Point3D(p.x() + s.dXp, p.y() + s.dYp, p.z() + s.dZp); }

private:
    doubli dXp, dYp, dZp; // delta
};

/*****************************************************************************
  HRect3D : a horizontal cuboid wich can't rotate
    => the faces are parallel to Oxy, Oxz & Oyz
    Point3D : point of the middle
    Size3D : the size between two opposite points
 *****************************************************************************/

class HRect3D : public Point3D, public Size3D
{
public:
    constexpr inline HRect3D() {}
    constexpr inline HRect3D(const Point3D &pA, const Point3D &pB)
        : Point3D(pA + Size3D(pA, pB) / 2), Size3D(pA, pB),
          pMin(min(pA.x(), pB.x()), min(pA.y(), pB.y()), min(pA.z(), pB.z())),
          pMax(max(pA.x(), pB.x()), max(pA.y(), pB.y()), max(pA.z(), pB.z())) {}
    constexpr inline HRect3D(const Point3D &pMid, const Size3D &s)
        : Point3D(pMid), Size3D(s), pMin(pMid - s / 2), pMax(pMid + s / 2) {}
    constexpr inline HRect3D(const HRect3D &r) : Point3D(r), Size3D(r), pMin(r.pMin), pMax(r.pMax) {}

    constexpr inline const Size3D &getSize() const { return *this; }
    constexpr inline const Point3D &getPointMin() const { return pMin; }
    constexpr inline const Point3D &getPointMax() const { return pMax; }
    constexpr inline const Point3D &getMiddle() const { return *this; }

    void scale(const doubli &scale);
    constexpr inline HRect3D operator+(const Point3D &pointAdd) const { return HRect3D{static_cast<Point3D>(*this) + pointAdd, static_cast<Size3D>(*this)}; }
    constexpr inline HRect3D operator-(const Point3D &pointAdd) const { return HRect3D{static_cast<Point3D>(*this) - pointAdd, static_cast<Size3D>(*this)}; }
    constexpr inline HRect3D operator+(const Size3D &sizeAdd) const { return HRect3D{static_cast<Point3D>(*this) + sizeAdd, static_cast<Size3D>(*this)}; }
    constexpr inline HRect3D operator-(const Size3D &sizeAdd) const { return HRect3D{static_cast<Point3D>(*this) - sizeAdd, static_cast<Size3D>(*this)}; }
    constexpr inline HRect3D *operator=(const HRect3D &rect);

    bool operator==(const HRect3D &rect) const;
    constexpr inline bool contains(const Point3D &point) const;
    bool containsLine(const Point3D &pA, const Vec3D &vDir, const doubli &dMax) const;

    friend QDebug operator<<(QDebug debug, const HRect3D &rect);

private:
    Point3D pMin, pMax;
};

constexpr inline HRect3D *HRect3D::operator=(const HRect3D &rect)
{
    pMin = rect.pMin;
    pMax = rect.pMax;
    return this;
}

constexpr inline bool HRect3D::contains(const Point3D &point) const
{
    return pMin.x() <= point.x() && point.x() <= pMax.x()
           && pMin.y() <= point.y() && point.y() <= pMax.y()
           && pMin.z() <= point.z() && point.z() <= pMax.z();
}

/*****************************************************************************
  Rect3D : a cuboid wich can rotate
    TODO: to rotate a Solid
 *****************************************************************************/

class Rect3D : public HRect3D
{
public:
    constexpr inline Rect3D() : HRect3D() {}
    constexpr inline Rect3D(const Point3D &pA, const Point3D &pB, const Point3D &pC) : HRect3D(pA, pC), pB(pB) { calcMinMax(); }
    constexpr inline Rect3D(const Point3D &pA, const Point3D &pB, const Size3D &s) : HRect3D(pA, s), pB(pB) { calcMinMax(); }
    constexpr inline Rect3D(const Rect3D &r) : HRect3D(r), pB(r.pB), pMin(r.pMin), pMax(r.pMax) {}

    constexpr inline Size3D getSize() const { return Size3D{pMin, pMax}; }
    constexpr inline const Point3D &getPointMin() const { return pMin; }
    constexpr inline const Point3D &getPointMax() const { return pMax; }
    constexpr inline Point3D getMiddle() const { return (pMin + pMax) / 2; }

    //void setPointMin(const Point3D &pointMin) { this->pointMin = pointMin; }//pas recommandé
    //void setPointMax(const Point3D &pointMax) { this->pointMax = pointMax; }//pas recommandé
    void scale(const doubli &scale);
    Rect3D operator+(const Point3D &pAdd) const { return Rect3D{pA + pAdd, pB + pAdd, pC + pAdd}; }
    Rect3D *operator=(const Rect3D &rect);
    bool operator==(const Rect3D &rect) const;

    bool contains(const Point3D &point) const;

    friend QDebug operator<<(QDebug debug, const Rect3D &rect);

private:
    Point3D pA, pB, pC, pMin, pMax;
    constexpr void calcMinMax();
    // TODO pb: on a pas assez d'info, la on a qu'une droite de modélisé, pas du tout un rectangle
};

constexpr void Rect3D::calcMinMax()
{
    pMax = Point3D(max(HRect3D::getPointMax().x(), pB.x()),
                   max(HRect3D::getPointMax().y(), pB.y()),
                   max(HRect3D::getPointMax().z(), pB.z()));
    pMin = Point3D(min(HRect3D::getPointMin().x(), pB.x(), pC.x()),
                   min(HRect3D::getPointMin().y(), pB.y()),
                   min(HRect3D::getPointMin().z(), pB.z()));
}

/*****************************************************************************
  Plan
    a, b, c, d : cartesian equation of a plane
    (a, b, c) is a unit normal of the plane
    pA : a point of the plan (to calc faster)
 *****************************************************************************/

class Plan
{
public:
    constexpr inline Plan() : pA() {}
    inline Plan(const Point3D &pA, const Point3D &pB, const Point3D &pC) : pA(pA) { calcEquation(pB, pC); }
    Plan(const HRect3D &r);
    constexpr inline Plan(const Plan &p) : pA(p.pA), a(p.a), b(p.b), c(p.c), d(p.d) {}
    Plan *operator=(const Plan &p);
    // mais avec a = 1 ! (colinéaire donc pas besoin de chercher plus...)
    constexpr inline bool paralleleDroite(const Size3D &vect) const;
    Point3DCancelable intersection(const Point3D &pA, const Point3D &pB) const;
    QPointF projeteOrtho(const Point3D &pA) const;
    Point3D projeteOrtho3D(const Point3D &pA) const;
    constexpr inline bool isValid() const { return a != 0.0L || b != 0.0L || c != 0.0L || d != 0.0L; }
    bool containsPoint(const Point3D &point) const;

    // sens positif (ne plus utiliser)
    //radian getRX() const { return a >= 0 ? atan(a / b) : M_PI + atan(a / b); }
    //radian getRZ() const { return atan(c / sqrt(a * a + b * b)); }
    constexpr inline Vec3D normale() const { return Vec3D{a, b, c}; } // (calculé unitaire)

private:
    Point3D pA;
    doubli a = 0, b = 0, c = 0, d = 0;
    void calcEquation(const Point3D &pB, const Point3D &pC); // ax + by + cz + d = 0
};

constexpr inline bool Plan::paralleleDroite(const Size3D &vect) const
{
    // voir #Orthogonal
    // scalaireAvecVectNormal, plus opti qu'avec un Vec3D
    return (a * vect.dX() + b * vect.dY() + c * vect.dZ()) == 0;
}

#endif // GEOMETRY3D_H
