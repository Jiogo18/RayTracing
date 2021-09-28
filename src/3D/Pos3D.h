#ifndef POS3D_H
#define POS3D_H

#include "Point3D.h"

/*****************************************************************************
  Rot3D
 *****************************************************************************/

class Rot3D
{
public:
    constexpr inline Rot3D() : rXp(0), rZp(0) {}
    constexpr inline Rot3D(const radian &rX, const radian &rZ) : rXp(rX), rZp(rZ) {}
    // constructeur de copie 2* plus rapide
    static inline Rot3D fromVector(const Point3D &v);

    constexpr inline const radian &rX() const { return rXp; }
    constexpr inline const radian &rZ() const { return rZp; }
    constexpr inline const Rot3D &getRot() const { return *this; }
    inline Vec3D toVector() const; // vecteur unitaire

    constexpr inline void setRX(const radian &rX) { rXp = rX; }
    constexpr inline void setRZ(const radian &rZ) { rZp = rZ; }
    constexpr inline void addRX(const radian &rX) { rXp += rX; }
    constexpr inline void addRZ(const radian &rZ) { rZp += rZ; }
    constexpr inline void setRot(const Rot3D &rot);

    bool operator==(const Rot3D &rot) const;
    bool operator!=(const Rot3D &rot) const;
    bool isNan() const;

    friend std::ostream &operator<<(std::ostream &os, const Rot3D &point);

private:
    radian rXp, rZp;
};

/*****************************************************************************
  Rot3D : constexpr & inline (big) functions
 *****************************************************************************/

inline Rot3D Rot3D::fromVector(const Point3D &v)
{
    //https://fr.wikipedia.org/wiki/Coordonn%C3%A9es_polaires
    //https://wikimedia.org/api/rest_v1/media/math/render/svg/d4a6b8b7e93b05abc00d7789635a762b495d0d67
#ifdef NAN_VERIF
    const radian rX = 2 * atan(y / (x + sqrt(x * x + y * y))), rZ = asin(z / vect.distanceOrigine());
    if (isnanf(rX) || isnanf(rZ)) {
        std::cout << "Rot3D::fromVector is nan: " << rX << " " << rZ << " " << vect << std::endl;
        exit(-1);
    }
#else
    return Rot3D{static_cast<radian>(atan2(v.y(), v.x())), static_cast<radian>(asin(v.z() / v.distanceOrigine()))};
    //return Rot3D{ 2 * atan(v.y() / (v.x() + v.distanceAxeZ())), asin(v.z() / v.distanceOrigine()) };
#endif
}

inline Vec3D Rot3D::toVector() const { return Vec3D{cosTaylorMin(rXp) * cosTaylorMin(rZp), sinTaylorMin(rXp) * cosTaylorMin(rZp), sinTaylorMin(rZp)}; }

constexpr inline void Rot3D::setRot(const Rot3D &rot)
{
    rXp = rot.rXp;
    rZp = rot.rZp;
}

/*****************************************************************************
  Pos3D
  The lowest definition of an object : a position and a direction
 *****************************************************************************/

class Pos3D : public Point3D, public Rot3D
{
public:
    constexpr inline Pos3D() : Point3D(), Rot3D() {}
    constexpr inline Pos3D(const doubli &x, const doubli &y, const doubli &z,
                           const radian &rX, const radian &rZ) : Point3D(x, y, z), Rot3D(rX, rZ) {}
    constexpr inline Pos3D(const Point3D &p,
                           const radian &rX, const radian &rZ) : Point3D(p), Rot3D(rX, rZ) {}
    constexpr inline Pos3D(const Point3D &p, const Rot3D &r) : Point3D(p), Rot3D(r) {}
    constexpr inline Pos3D(const Pos3D &p) : Point3D(p), Rot3D(p) {}
    static Pos3D fromDegree(const doubli &x, const doubli &y, const doubli &z,
                            const radian &rX, const radian &rZ);

    constexpr Pos3D *operator=(const Pos3D &p);
    constexpr void setPos(const Pos3D &p);

    void moveWithRot(const doubli &speed, const radian &rot);
    static Point3D pointFromRot(const doubli &d, const radian &rX, const radian &rZ);

    constexpr const Pos3D &getPos() const { return *this; }
    constexpr inline Point3D getNextPoint() const;
    constexpr inline Point3D getNextPointRelatif() const;
    Pos3D getChildRot(const radian &rXRelatif, const radian &rZRelatif) const;
    Point3D changeRef(const Point3D &point, const doubli &srX, const doubli &srZ) const;

    bool operator==(const Pos3D &pos) const;
    bool operator!=(const Pos3D &pos) const;
    bool isNan() const;

    friend std::ostream &operator<<(std::ostream &os, const Pos3D &pos);

private:
    static Pos3D getRotAsVect(const Point3D &p1, const Point3D &p2);
    static Pos3D getRotAsPoint(const Point3D &p);
};

/*****************************************************************************
  Pos3D : constexpr & inline (big) functions
 *****************************************************************************/

constexpr inline Pos3D *Pos3D::operator=(const Pos3D &p)
{
    Point3D::operator=(p);
    Rot3D::operator=(p);
    return this;
}

constexpr inline void Pos3D::setPos(const Pos3D &p)
{
    Point3D::operator=(p);
    Rot3D::operator=(p);
}

constexpr inline Point3D Pos3D::getNextPoint() const
{
    return Point3D{cosTaylorMin(rZ()) * cosTaylorMin(rX()) + x(), cosTaylorMin(rZ()) * sinTaylorMin(rX()) + y(), sinTaylorMin(rZ()) + z()};
}

constexpr inline Point3D Pos3D::getNextPointRelatif() const { return Point3D{cosTaylorMin(rZ()) * cosTaylorMin(rX()), cosTaylorMin(rZ()) * sinTaylorMin(rX()), sinTaylorMin(rZ())}; }

#endif // POS3D_H
