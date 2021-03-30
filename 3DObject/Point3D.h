#ifndef POINT3D_H
#define POINT3D_H

#include <QDebug>
#include <QtMath>

//#define NAN_VERIF//activer pour le debug des nan

typedef long double doubli;//un nombre pour gérer 6 décimales max (arrondit)
bool isNull(const doubli d);
doubli roundNull(const doubli d);//plus opti que round
constexpr doubli round(const doubli d) { return std::floor(d * 1000000 + 0.5L) / 1000000; }
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
    constexpr inline Point3D() : x(0), y(0), z(0) {}
    constexpr inline Point3D(const doubli& x, const doubli& y, const doubli& z) : x(round(x)), y(round(y)), z(round(z)) {}

    inline doubli getX() const { return x; }
    inline doubli getY() const { return y; }
    inline doubli getZ() const { return z; }
    Point3D getPoint() const { return *this; }
    inline void setX(doubli x) { this->x = x; }
    inline void setY(doubli y) { this->y = y; }
    inline void setZ(doubli z) { this->z = z; }
    inline void addX(doubli x) { this->x += x; }
    inline void addY(doubli y) { this->y += y; }
    inline void addZ(doubli z) { this->z += z; }
    void setPoint(const Point3D& point) { x = point.x; y = point.y; z = point.z; }

    doubli distanceAxeZ() const { return std::sqrt(x * x + y * y); }
    bool operator <(const Point3D& point) const;
    bool operator >(const Point3D& point) const;
    bool operator ==(const Point3D& point) const;
    bool operator !=(const Point3D& point) const;
    bool operator <=(const Point3D& point) const;
    bool operator >=(const Point3D& point) const;
    bool isNull() const { return x == 0.0L && y == 0.0L && z == 0.0L; }
    bool isInf() const { return qIsInf(x) || qIsInf(y) || qIsInf(z); }
    bool isNan() const { return isnanf(x) || isnanf(y) || isnanf(z); }
    static Point3D makeInfinite() { return Point3D(INFINITY, INFINITY, INFINITY); }

    Point3D operator +(const Point3D& point) const;
    Point3D operator -(const Point3D& point) const;
    Point3D operator *(const Point3D& point) const;
    Point3D operator *(doubli n) const;
    Point3D operator /(const Point3D& point) const;
    Point3D operator /(doubli n) const;
    // unary operator
    friend inline Point3D operator-(const Point3D& p) { return Point3D(-p.x, -p.y, -p.z); }

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
};

class Point3DCancelable : public Point3D {
public:
    constexpr inline Point3DCancelable() : valid(false) {}
    constexpr inline Point3DCancelable(doubli x, doubli y, doubli z) : Point3D(x, y, z), valid(true) {}
    constexpr inline Point3DCancelable(const Point3D& p) : Point3D(p), valid(true) {}
    constexpr inline bool isValid() const { return valid; }
private:
    const bool valid;
};

class Vec3D : public Point3D
{
public:
    Vec3D(doubli x, doubli y, doubli z) { this->x = x; this->y = y; this->z = z; }// plus opti
    doubli produitScalaire(const Vec3D& vec) const { return x * vec.x + y * vec.y + z * vec.z; }

    // unary operator
    friend inline Vec3D operator-(const Vec3D& p) { return Vec3D(-p.x, -p.y, -p.z); }
};


#endif //POINT3D_H