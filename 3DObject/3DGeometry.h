#ifndef GEOMETRY3D_H
#define GEOMETRY3D_H


#include <QDebug>
#include <QtMath>
#include <QPointF>
#include "DebugTime.h"


typedef long double doubli;//un nombre pour gérer 6 décimales max (arrondit)
bool isNull(const doubli& d);
doubli roundNull(const doubli& d);//plus opti que round
doubli round(const doubli& d);
doubli signOf(const doubli& d);
typedef float radian;
doubli sqr(const doubli& d);//easy
radian degreesToRadians(const doubli& deg);
doubli radiansToDegrees(const radian& rad);
doubli sqrt(const doubli& d);
doubli cos(const radian& deg);
doubli sin(const radian& deg);
doubli tan(const radian& deg);
radian acos(const doubli& d);
radian asin(const doubli& d);
radian atan(const doubli& d);
doubli qIsInf(const doubli& d);
doubli max(const doubli& a, const doubli& b);
doubli max(const doubli& a, const doubli& b, const doubli& c);
doubli min(const doubli& a, const doubli& b);
doubli min(const doubli& a, const doubli& b, const doubli& c);
bool isNull(const radian& d);
radian mod(const radian& n, const radian& d);//perfect return : [0;d[ & 3 times faster
radian modRad(radian d);//intervalle radian : ]-PI; PI]
radian roundNull(radian d);//plus opti que round
float signOf(radian d);// -1 or 1
QDebug operator <<(QDebug debug, const doubli& d);

class Point3D
{
public:
    Point3D();
    Point3D(doubli x, doubli y, doubli z);
    Point3D(const Point3D& point);
    Point3D* operator =(const Point3D& point);

    doubli getX() const { return x; }
    doubli getY() const { return y; }
    doubli getZ() const { return z; }
    void setX(doubli x) { this->x = round(x); }
    void setY(doubli y) { this->y = round(y); }
    void setZ(doubli z) { this->z = round(z); }
    void addX(doubli x) { this->x = round(this->x + x); }
    void addY(doubli y) { this->y = round(this->y + y); }
    void addZ(doubli z) { this->z = round(this->z + z); }

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
    static Point3D inifinite() { return Point3D(INFINITY, INFINITY, INFINITY); }

    Point3D operator +(const Point3D& point) const;
    Point3D operator -(const Point3D& point) const;
    Point3D operator *(const Point3D& point) const;
    Point3D operator *(doubli n) const;
    Point3D operator /(const Point3D& point) const;
    Point3D operator /(doubli n) const;

    static doubli distance(const Point3D& pA, const Point3D& pB);
    static doubli distanceMax(const Point3D& pA, const Point3D& pB);

    friend QDebug operator <<(QDebug debug, const Point3D& point);
    friend Point3D qFloor(const Point3D& point);
    friend Point3D qCeil(const Point3D& point);

private:
    doubli x;
    doubli y;
    doubli z;
    bool defined = false;
};

class Pos3D : public Point3D {
public:
    Pos3D();
    Pos3D(doubli x, doubli y, doubli z, radian rX, radian rZ);
    Pos3D(const Point3D& point, radian rX, radian rZ);
    Pos3D(const Pos3D& pos);
    Pos3D* operator=(const Pos3D& pos);
    static Pos3D fromDegree(doubli x, doubli y, doubli z, radian rX, radian rZ);

    radian getRX() const { return rX; }
    radian getRZ() const { return rZ; }
    void setRX(radian rX) { this->rX = rX; }
    void setRZ(radian rZ) { this->rZ = rZ; }
    void addRX(radian rX) { this->rX += rX; }
    void addRZ(radian rZ) { this->rZ += rZ; }

    void moveWithRot(doubli speed, radian rot);
    static Point3D pointFromRot(doubli d, radian rX, radian rZ);
    Point3D getNextPointRelatif() const;
    Point3D getNextPoint() const;
    Pos3D getChildRot(radian rXRelatif, radian rZRelatif) const;
    Point3D changeRef(const Point3D& point) const;
    bool operator ==(const Pos3D& pos) const;
    bool operator !=(const Pos3D& pos) const;

    friend QDebug operator << (QDebug debug, const Pos3D& pos);
private:
    radian rX;
    radian rZ;

    static Point3D rotation(Point3D point, radian rX, radian rZ);
    static doubli rotation1(doubli x, doubli y, doubli sR, doubli cR);
    static doubli rotation2(doubli x, doubli y, doubli sR, doubli cR);
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
    void setDX(doubli dZ) { this->dZ = round(dZ); }
    void setDY(doubli dY) { this->dY = round(dY); }
    void setDZ(doubli dZ) { this->dZ = round(dZ); }
    Size3D operator *(doubli scale) const { return Size3D(dX * scale, dY * scale, dZ * scale); }
    Size3D operator /(doubli scale) const { return Size3D(dX / scale, dY / scale, dZ / scale); }
protected:
    doubli dX;//delta X, width
    doubli dY;//delta Y, height
    doubli dZ;//delta Z, depth
};
Point3D operator -(const Point3D& p, const Size3D& s);
Point3D operator +(const Point3D& p, const Size3D& s);

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
    Point3D pointMax;
    Point3D pointMin;
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
    Point3D pointA;
    Point3D pointB;
    Point3D pointC;
    Point3D pointMax;
    Point3D pointMin;
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
    inline bool isValid() const { return a != 0.0L || b != 0.0L || c != 0.0L || d != 0.0L; }
    bool containsPoint(const Point3D& point) const;
private:
    Point3D pA;
    Point3D pB;
    Point3D pC;
    doubli a = 0, b = 0, c = 0, d = 0;
    void calcEquation();//ax + by + cz + d = 0
};



#endif // GEOMETRY3D_H
