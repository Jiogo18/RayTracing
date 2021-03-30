#ifndef POS3D_H
#define POS3D_H

#include "Point3D.h"

class Rot3D
{
public:
    constexpr inline Rot3D() : rXp(0), rZp(0) {}
    constexpr inline Rot3D(const radian &rX, const radian &rZ) : rXp(rX), rZp(rZ) {}
    constexpr inline Rot3D(const Rot3D &rot) : rXp(rot.rXp), rZp(rot.rZp) {}
    // constructeur de copie 2* plus rapide
    static Rot3D fromVector(const Point3D &v);

    constexpr inline const radian &rX() const { return rXp; }
    constexpr inline const radian &rZ() const { return rZp; }
    constexpr inline const Rot3D &getRot() const { return *this; }
    Vec3D toVector() const; // vecteur unitaire

    constexpr inline void setRX(const radian &rX) { rXp = rX; }
    constexpr inline void setRZ(const radian &rZ) { rZp = rZ; }
    constexpr inline void addRX(const radian &rX) { rXp += rX; }
    constexpr inline void addRZ(const radian &rZ) { rZp += rZ; }
    constexpr inline void setRot(const Rot3D &rot)
    {
        rXp = rot.rXp;
        rZp = rot.rZp;
    }

    bool operator==(const Rot3D &rot) const;
    bool operator!=(const Rot3D &rot) const;
    bool isNan() const;

    friend QDebug operator<<(QDebug debug, const Rot3D &point);

private:
    radian rXp, rZp;
};

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

    void moveWithRot(const doubli &speed, const radian &rot);
    static Point3D pointFromRot(const doubli &d, const radian &rX, const radian &rZ);
    Point3D getNextPoint() const;
    Pos3D getChildRot(const radian &rXRelatif, const radian &rZRelatif) const;
    Point3D changeRef(const Point3D &point, const doubli &srX, const doubli &srZ) const;

    bool operator==(const Pos3D &pos) const;
    bool operator!=(const Pos3D &pos) const;
    bool isNan() const;

    friend QDebug operator<<(QDebug debug, const Pos3D &pos);

private:
    static Pos3D getRotAsVect(const Point3D &p1, const Point3D &p2);
    static Pos3D getRotAsPoint(const Point3D &p);
};

#endif // POS3D_H
