#ifndef GEOMETRY3D_H
#define GEOMETRY3D_H



#include <QPointF>
#include "DebugTime.h"
#include "Pos3D.h"

class Size3D
{
public:
    Size3D();
    Size3D(doubli dX, doubli dY, doubli dZ);
    Size3D(const Size3D& size);
    Size3D(const Point3D& pA, const Point3D& pB);
    doubli getDX() const { return dX; }
    doubli getDY() const { return dY; }
    doubli getDZ() const { return dZ; }
    Size3D getSize() const { return *this; }
    Point3D toPoint() const { return Point3D(dX, dY, dZ); }//relative to (0,0,0)
    void setDX(doubli dX) { this->dX = dX; }
    void setDY(doubli dY) { this->dY = dY; }
    void setDZ(doubli dZ) { this->dZ = dZ; }
    Size3D operator *(doubli scale) const { return Size3D(dX * scale, dY * scale, dZ * scale); }
    Size3D operator /(doubli scale) const { return Size3D(dX / scale, dY / scale, dZ / scale); }

    friend Point3D operator -(const Point3D& p, const Size3D& s);
    friend Point3D operator +(const Point3D& p, const Size3D& s);

protected:
    doubli dX, dY, dZ;//delta
};


class HRect3D
{
public:
    HRect3D();// a horizontal cuboid
    HRect3D(const Point3D& pointA, const Point3D& pointB);
    HRect3D(const Point3D& pointA, const Size3D& size);
    HRect3D(const HRect3D& rect);
    Size3D getSize() const { return Size3D(pointMin, pointMax); }
    Point3D getPointMin() const { return pointMin; }
    Point3D getPointMax() const { return pointMax; }
    Point3D getMiddle() const { return (pointMin + pointMax) / 2; }

    //void setPointMin(const Point3D &pointMin) { this->pointMin = pointMin; }//pas recommandé
    //void setPointMax(const Point3D &pointMax) { this->pointMax = pointMax; }//pas recommandé
    void scale(doubli scale);
    HRect3D operator +(const Point3D& pointAdd) const { return HRect3D(pointMin + pointAdd, pointMax + pointAdd); }
    HRect3D* operator =(const HRect3D& rect);
    bool operator ==(const HRect3D& rect) const;

    bool contains(const Point3D& point) const;

    friend QDebug operator << (QDebug debug, const HRect3D& rect);

private:
    Point3D pointMin, pointMax;
    //TODO pb: on a pas assez d'info, la on a qu'une droite de modélisé, pas du tout un rectangle
};


class Rect3D : public HRect3D
{
public:
    Rect3D();
    Rect3D(const Point3D& pointA, const Point3D& pointB, const Point3D& pointC);
    Rect3D(const Point3D& pointA, const Point3D& pointB, const Size3D& size);
    Rect3D(const Rect3D& rect);
    Size3D getSize() const { return Size3D(pointMin, pointMax); }
    Point3D getPointMin() const { return pointMin; }
    Point3D getPointMax() const { return pointMax; }
    Point3D getMiddle() const { return (pointMin + pointMax) / 2; }

    //void setPointMin(const Point3D &pointMin) { this->pointMin = pointMin; }//pas recommandé
    //void setPointMax(const Point3D &pointMax) { this->pointMax = pointMax; }//pas recommandé
    void scale(doubli scale);
    Rect3D operator +(const Point3D& pointAdd) const { return Rect3D(pointA + pointAdd, pointB + pointAdd, pointC + pointAdd); }
    Rect3D* operator =(const Rect3D& rect);
    bool operator ==(const Rect3D& rect) const;

    bool contains(const Point3D& point) const;

    friend QDebug operator << (QDebug debug, const Rect3D& rect);

private:
    Point3D pointA, pointB, pointC, pointMax, pointMin;
    void calcMinMax();
    //TODO pb: on a pas assez d'info, la on a qu'une droite de modélisé, pas du tout un rectangle
};




class Plan
{
public:
    Plan();
    Plan(const Point3D& pA, const Point3D& pB, const Point3D& pC);
    Plan(const HRect3D& rect);
    Plan(const Plan& plan);
    Plan* operator =(const Plan& plan);
    //mais avec a = 1 ! (colinéaire donc pas besoin de chercher plus...)
    bool paralleleDroite(const Size3D& vect) const;
    Point3DCancelable intersection(const Point3D& pA, const Point3D& pB) const;
    QPointF projeteOrtho(const Point3D& pA) const;
    Point3D projeteOrtho3D(const Point3D& pA) const;
    inline bool isValid() const { return a != 0.0L || b != 0.0L || c != 0.0L || d != 0.0L; }
    bool containsPoint(const Point3D& point) const;

    // sens positif (ne plus utiliser)
    //radian getRX() const { return a >= 0 ? atan(a / b) : M_PI + atan(a / b); }
    //radian getRZ() const { return atan(c / sqrt(a * a + b * b)); }
    Vec3D normale() const { return Vec3D(a, b, c); }
    Vec3D normaleUnitaire() const { return Vec3D(a, b, c); }

private:
    Point3D pA;
    doubli a = 0, b = 0, c = 0, d = 0;
    void calcEquation(const Point3D& pB, const Point3D& pC);//ax + by + cz + d = 0
};



#endif // GEOMETRY3D_H
