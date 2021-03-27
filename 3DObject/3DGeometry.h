#ifndef GEOMETRY3D_H
#define GEOMETRY3D_H


#include <QDebug>
#include <QtMath>
#include <QPointF>
#include "DebugTime.h"

//#define NAN_VERIF//activer pour le debug des nan

typedef long double doubli;//un nombre pour gérer 6 décimales max (arrondit)
bool isNull(const doubli d);
doubli roundNull(const doubli d);//plus opti que round
doubli round(const doubli d);
doubli signOf(const doubli d);
typedef float radian;
doubli sqr(const doubli d);//easy
radian degreesToRadians(const doubli deg);
doubli radiansToDegrees(const radian rad);
doubli sqrt(const doubli d);
doubli cos(const radian deg);
doubli sin(const radian deg);
doubli tan(const radian deg);
radian acos(const doubli d);
radian asin(const doubli d);
radian atan(const doubli d);
doubli qIsInf(const doubli d);
doubli max(const doubli a, const doubli b);
doubli max(const doubli a, const doubli b, const doubli c);
doubli min(const doubli a, const doubli b);
doubli min(const doubli a, const doubli b, const doubli c);
bool isNull(const radian d);
radian mod(const radian n, const radian d);//perfect return : [0;d[ & 3 times faster
radian modRad(radian d);//intervalle radian : ]-PI; PI]
radian roundNull(radian d);//plus opti que round
float signOf(radian d);// -1 or 1
QDebug operator <<(QDebug debug, const doubli d);

class Point3D
{
public:
    Point3D();
    Point3D(doubli x, doubli y, doubli z);
    Point3D(const Point3D& point);
    Point3D* operator =(const Point3D& point);

    inline doubli getX() const { return x; }
    inline doubli getY() const { return y; }
    inline doubli getZ() const { return z; }
    Point3D getPoint() const { return *this; }
    void setX(doubli x) { this->x = x; }
    void setY(doubli y) { this->y = y; }
    void setZ(doubli z) { this->z = z; }
    void addX(doubli x) { this->x += x; }
    void addY(doubli y) { this->y += y; }
    void addZ(doubli z) { this->z += z; }
    void setPoint(const Point3D& point) { x = point.x; y = point.y; z = point.z; }

    doubli distanceAxeZ() const { return std::sqrt(x * x + y * y); }
    bool operator <(const Point3D& point) const;
    bool operator >(const Point3D& point) const;
    bool operator ==(const Point3D& point) const;
    bool operator !=(const Point3D& point) const;
    bool operator <=(const Point3D& point) const;
    bool operator >=(const Point3D& point) const;
    bool isNull() const { return x == 0.0L && y == 0.0L && z == 0.0L; }
    bool isValid() const { return defined; }
    bool isInf() const { return qIsInf(x) || qIsInf(y) || qIsInf(z); }
    bool isNan() const { return isnanf(x) || isnanf(y) || isnanf(z); }
    static Point3D makeInfinite() { return Point3D(INFINITY, INFINITY, INFINITY); }
    static Point3D makeNotDefined() { Point3D p; p.defined = false; return p; }

    Point3D operator +(const Point3D& point) const;
    Point3D operator -(const Point3D& point) const;
    Point3D operator *(const Point3D& point) const;
    Point3D operator *(doubli n) const;
    Point3D operator /(const Point3D& point) const;
    Point3D operator /(doubli n) const;

    static doubli distance(const Point3D& pA, const Point3D& pB);
    static doubli distanceMax(const Point3D& pA, const Point3D& pB);
    doubli distance(const Point3D& pA) const;
    doubli distanceMax(const Point3D& pA) const;
    doubli distanceOrigine() const { return sqrt(x * x + y * y + z * z); }

    friend QDebug operator <<(QDebug debug, const Point3D& point);
    friend Point3D qFloor(const Point3D& point);
    friend Point3D qCeil(const Point3D& point);

protected:
    doubli x, y, z;
    bool defined = true;
};

class Vec3D : public Point3D
{
public:
    Vec3D(doubli x, doubli y, doubli z) { this->x = x; this->y = y; this->z = z; }// plus opti
    doubli produitScalaire(const Vec3D& vec) const { return x * vec.x + y * vec.y + z * vec.z; }
};

class Rot3D
{
public:
    Rot3D() { rX = 0; rZ = 0; }
    Rot3D(radian rX, radian rZ) { this->rX = rX; this->rZ = rZ; }
    Rot3D(const Rot3D& rot) { rX = rot.rX; rZ = rot.rZ; }
    static Rot3D fromVector(const Point3D& vect);

    radian getRX() const { return rX; }
    radian getRZ() const { return rZ; }
    Rot3D getRot() const { return *this; }
    Vec3D toVector() const;
    Vec3D toVectorU() const;// vecteur unitaire

    void setRX(radian rX) { this->rX = rX; }
    void setRZ(radian rZ) { this->rZ = rZ; }
    void addRX(radian rX) { this->rX += rX; }
    void addRZ(radian rZ) { this->rZ += rZ; }
    void setRot(const Rot3D& rot) { rX = rot.rX; rZ = rot.rZ; }

    bool operator ==(const Rot3D& rot) const { return rX == rot.rX && rZ == rot.rZ; }
    bool operator !=(const Rot3D& rot) const { return rX != rot.rX || rZ != rot.rZ; }
    bool isNan() const { return isnanf(rX) || isnanf(rZ); }

    friend QDebug operator <<(QDebug debug, const Rot3D& point);

protected:
    radian rX, rZ;
};

class Pos3D : public Point3D, public Rot3D
{
public:
    Pos3D();
    Pos3D(doubli x, doubli y, doubli z, radian rX, radian rZ);
    Pos3D(const Point3D& point, radian rX, radian rZ);
    Pos3D(const Point3D& point, const Rot3D& rot);
    Pos3D(const Pos3D& pos);
    Pos3D* operator=(const Pos3D& pos);
    static Pos3D fromDegree(doubli x, doubli y, doubli z, radian rX, radian rZ);

    void moveWithRot(doubli speed, radian rot);
    static Point3D pointFromRot(doubli d, radian rX, radian rZ);
    Point3D getNextPointRelatif() const { return toVector(); }
    Point3D getNextPoint() const;
    Pos3D getChildRot(radian rXRelatif, radian rZRelatif) const;
    Point3D changeRef(const Point3D point, doubli srX, doubli srZ) const;

    bool operator ==(const Pos3D& pos) const;
    bool operator !=(const Pos3D& pos) const;
    bool isNan() const { return isnanf(x) || isnanf(y) || isnanf(z) || isnanf(rX) || isnanf(rZ); }

    friend QDebug operator << (QDebug debug, const Pos3D& pos);
private:

    static Pos3D getRotAsVect(const Point3D& p1, const Point3D& p2);
    static Pos3D getRotAsPoint(const Point3D& p);

};

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
    Point3D intersection(const Point3D& pA, const Point3D& pB) const;
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
