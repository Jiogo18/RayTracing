#ifndef POS3D_H
#define POS3D_H

#include "Point3D.h"




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



#endif // POS3D_H