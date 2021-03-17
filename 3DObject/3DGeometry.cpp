#include "3DGeometry.h"

doubli sqr(const doubli &d) { return d*d; }
radiant degreesToRadians(const doubli &deg) { return fmodl(deg, 360) * doubli(M_PI) / 180; }
doubli radiansToDegrees(const radiant &rad) { return rad * 180 / doubli(M_PI); }
doubli sqrt(const doubli &d) { return std::sqrt(d); }
doubli cos(const radiant &deg) { return std::cos(deg); }
doubli sin(const radiant &deg) { return std::sin(deg); }
doubli tan(const radiant &deg) { return std::tan(deg); }
radiant acos(const doubli &d) { return std::acos(d); }
radiant asin(const doubli &d) { return std::asin(d); }
radiant atan(const doubli &d) { return std::atan(d); }
doubli qIsInf(const doubli &d) { return qIsInf(static_cast<double>(d)); }
QDebug operator <<(QDebug debug, const doubli &d) { debug << static_cast<double>(d); return debug; }


Point3D::Point3D() { x=0; y=0; z=0; }
Point3D::Point3D(doubli x, doubli y, doubli z)
{
    this->x = round(x);
    this->y = round(y);
    this->z = round(z);
    defined = true;
}
Point3D::Point3D(const Point3D &point)
{
    x = point.x;
    y = point.y;
    z = point.z;
    defined = point.defined;
}
Point3D *Point3D::operator =(const Point3D &point)
{
    x = point.x;
    y = point.y;
    z = point.z;
    defined = point.defined;
    return this;
}

bool Point3D::operator <(const Point3D &point) const
{
    //le plus petit = la plus proche
    if(z == point.z)
        return distanceAxeZ() < point.distanceAxeZ();//si plus proche axe Z => plus proche
    else
        return z < point.z;//si plus proche Z alors plus proche
}
bool Point3D::operator >(const Point3D &point) const
{
    //le plus grand = la plus loins
    if(z == point.z)
        return distanceAxeZ() > point.distanceAxeZ();//si plus loins axe Z => plus loins
    else
        return z > point.z;//si plus loins Z alors plus loins
}
bool Point3D::operator ==(const Point3D &point) const
{ return (x == point.x) && (y == point.y) && (z == point.z); }

Point3D Point3D::operator +(const Point3D &point) const
{
    return Point3D(x+point.x,
                   y+point.y,
                   z+point.z);
}
Point3D Point3D::operator -(const Point3D &point) const
{
    return Point3D(x-point.x,
                   y-point.y,
                   z-point.z);
}
Point3D Point3D::operator *(const Point3D &point) const
{
    return Point3D(x*point.x,
                   y*point.y,
                   z*point.z);
}
Point3D Point3D::operator *(doubli n) const { return Point3D(x*n, y*n, z*n); }
Point3D Point3D::operator /(const Point3D &point) const
{
    return Point3D(x/point.x,
                   y/point.y,
                   z/point.z);
}
Point3D Point3D::operator /(doubli n) const { return Point3D(x/n, y/n, z/n); }

doubli Point3D::distance(const Point3D &pA, const Point3D &pB)
{
    return sqrt(sqr(pB.x - pA.x)+
                sqr(pB.y - pA.y)+
                sqr(pB.z - pA.z));
}
doubli Point3D::distanceMax(const Point3D &pA, const Point3D &pB)
{
    doubli dX = abs(pB.x - pA.x),
           dY = abs(pB.y - pA.y),
           dZ = abs(pB.z - pA.z);
    if(dX < dY)
        dX = dY;
    return dX < dZ ? dZ : dX;//on prend le plus grand (ça forme un carré)
}

QDebug operator <<(QDebug debug, const Point3D &point)
{
    debug << "Point3D(" << point.x << "," << point.y << "," << point.z << ")";
    return debug;
}
Point3D qFloor(const Point3D &point)
{ return Point3D(std::floor(point.x), std::floor(point.y), std::floor(point.z)); }
Point3D qCeil(const Point3D &point)
{ return Point3D(std::ceil(point.x), std::ceil(point.y), std::ceil(point.z)); }






Pos3D::Pos3D() : Point3D(0, 0, 0) { rX = 0; rZ = 0; }
Pos3D::Pos3D(doubli x, doubli y, doubli z, radiant rX, radiant rZ) : Point3D(x, y, z)
{
    this->rX = rX; this->rZ = rZ;
}
Pos3D::Pos3D(const Point3D& point, radiant rX, radiant rZ) : Point3D(point)
{
    this->rX = rX; this->rZ = rZ;
}
Pos3D::Pos3D(const Pos3D &pos) : Point3D(pos) { rX = pos.rX; rZ = pos.rZ; }

Pos3D *Pos3D::operator=(const Pos3D &pos)
{
    Point3D::operator=(pos);
    rX = pos.rX;
    rZ = pos.rZ;
    return this;
}

Pos3D Pos3D::fromDegree(doubli x, doubli y, doubli z, radiant rX, radiant rZ)
{
    return Pos3D(x, y, z, degreesToRadians(rX), degreesToRadians(rZ));
}

void Pos3D::moveWithRot(doubli speed, radiant rot)
{
    //trigo, on connait l'hypothénuse (speed) et l'angle et on veut coté a ou b => cos ou sin
    addX(cos(rX+rot) * speed);
    addY(sin(rX+rot) * speed);
}

Point3D Pos3D::pointFromRot(doubli d, radiant rX, radiant rZ)
{
    return Point3D(cos(rZ)*cos(rX),
                   cos(rZ)*sin(rX),
                   sin(rZ) * d) * d;
}
Point3D Pos3D::getNextPointRelatif() const
{
    //on s'avance de 1 t
    //équivalent à pointFromRot(1, rX, rZ);
    return Point3D(cos(rZ)*cos(rX), cos(rZ)*sin(rX), sin(rZ));
}
Point3D Pos3D::getNextPoint() const
{
    return Point3D(cos(rZ)*cos(rX) + getX(),
                   cos(rZ)*sin(rX) + getY(),
                   sin(rZ) + getZ());
    //opti mais == getNextPointRelatif() + getPoint()
}

Pos3D Pos3D::getChildRot(radiant rXRelatif, radiant rZRelatif) const
{
    //changeRef de childNext
    doubli crZR = cos(rZRelatif),
            crZ = cos(rZ), crX = cos(rX),
            srZ = sin(rZ), srX = sin(rX);
    doubli x1 = crZR*cos(rXRelatif), y1 = crZR*sin(rXRelatif), z1 = sin(rZRelatif);//z1==srZR
    doubli x2 = x1*crZ - z1*srZ, z2 = x1*srZ + z1*crZ;//y2==y1
    doubli x3 = x2*crX - y1*srX, y3 = x2*srX + y1*crX;//z3==z2
    radiant rX = atan(y3/x3);
    if(x3 < 0) {//sinon on ne voit que tout ce qui est en x3>=0 (rotation de 180° du reste)
        rX += M_PI;//atan retourne -89.5 alors qu'il faut 90.5 (mais ça reste la meme chose)
        //pas besoin de différentier y car on est à k360° de diff
    }
    return Pos3D(getPoint(), rX, asin(z2));
}

Point3D Pos3D::changeRef(const Point3D &point) const { return rotation(point + getPoint(), rX, rZ); }

bool Pos3D::operator ==(const Pos3D &pos) const
{
    return Point3D::operator ==(static_cast<Point3D>(pos)) && rX == pos.rX && rZ == pos.rZ;
}

Point3D Pos3D::rotation(Point3D point, radiant rX, radiant rZ)
{
    //inversé par rapport aux autres progs (entre y et z et entre 1ere et 2eme ligne)
    doubli sR = sin(rZ), cR = cos(rZ);
    point = Point3D(rotation1(point.getX(), point.getZ(), sR, cR), point.getY(), rotation2(point.getX(), point.getZ(), sR, cR));
    sR = sin(rX); cR = cos(rX);
    return Point3D(rotation1(point.getX(), point.getY(), sR, cR), rotation2(point.getX(), point.getY(), sR, cR), point.getZ());
}
doubli Pos3D::rotation1(doubli x, doubli y, doubli sR, doubli cR) { return x*cR - y*sR; }
doubli Pos3D::rotation2(doubli x, doubli y, doubli sR, doubli cR) { return x*sR + y*cR; }
Pos3D Pos3D::getRotAsVect(const Point3D &p1, const Point3D &p2)
{
    radiant rZ = asin((p2.getZ()-p1.getZ())/Point3D::distance(p1, p2));//formule getNextPos z inversée
    radiant rX = atan((p2.getY()-p1.getY())/(p2.getX()-p1.getX()));
    if(p2.getX()-p1.getX() < 0) {
        if(p2.getY()-p1.getY() >= 0)
            rX += M_PI;//atan retourne -89.5 alors qu'il faut 90.5 (mais ça reste la meme chose)
        else
            rX -= M_PI;
    }
    return Pos3D(p1, rX, rZ);
}

Pos3D Pos3D::getRotAsPoint(const Point3D &p)
{
    //opti mais == getRotAsVect(Point3D(0,0,0),p)
    doubli d = Point3D::distance(Point3D(0,0,0), p);
    radiant rZ = asin((p.getZ())/d);//formule getNextPos z inversée
    radiant rX = atan((p.getY())/(p.getX()));
    if(p.getX() < 0) {
        if(p.getY() >= 0)
            rX += M_PI;//atan retourne -89.5 alors qu'il faut 90.5 (mais ça reste la meme chose)
        else
            rX -= M_PI;
    }
    return Pos3D(0, 0, 0, rX, rZ);
}


QDebug operator <<(QDebug debug, const Pos3D &pos)
{
    debug << "Pos3D(" << pos.getX() << "," << pos.getY() << "," << pos.getZ()
          << ";" << pos.getRX() << "," << pos.getRZ() << ")";
    return debug;
}



Size3D::Size3D(doubli width, doubli height, doubli depth)
{ this->width = width; this->height = height; this->depth = depth; }
Size3D::Size3D(const Size3D &size)
{ width = size.width; height = size.height; depth = size.depth; }
Size3D::Size3D(const Point3D &pA, const Point3D &pB)
{
    width = pB.getX() - pA.getX();
    height = pB.getY() - pA.getY();
    depth = pB.getZ() - pA.getZ();
}
Point3D operator -(const Point3D &p, const Size3D &s)
{ return Point3D(p.getX()-s.getWidth(), p.getY()-s.getHeight(), p.getZ()-s.getDepth()); }
Point3D operator +(const Point3D &p, const Size3D &s)
{ return Point3D(p.getX()+s.getWidth(), p.getY()+s.getHeight(), p.getZ()+s.getDepth()); }

Rect3D::Rect3D()
{
    pointMin = Point3D(0, 0, 0);
    pointMax = Point3D(0, 0, 0);
}
Rect3D::Rect3D(const Point3D &pointA, const Point3D &pointB)
{
    pointMin = pointA; pointMax = pointB;
    if(pointB.getX() < pointA.getX()) {
        pointMin.setX(pointB.getX());
        pointMax.setX(pointA.getX());
    }
    if(pointB.getY() < pointA.getY()) {
        pointMin.setY(pointB.getY());
        pointMax.setY(pointA.getY());
    }
    if(pointB.getZ() < pointA.getZ()) {
        pointMin.setZ(pointB.getZ());
        pointMax.setZ(pointA.getZ());
    }
}
Rect3D::Rect3D(const Point3D &pointA, const Size3D &size)
{ pointMin = pointA; pointMax = pointA + size; }
Rect3D::Rect3D(const Rect3D &rect)
{ pointMin = rect.pointMin; pointMax = rect.pointMax; }

void Rect3D::scale(double scale)
{
    Size3D demisizeScaled = getSize() / 2 * round(static_cast<doubli>(scale));
    pointMin = getMiddle() - demisizeScaled;
    pointMax = getMiddle() + demisizeScaled;
}

bool Rect3D::contains(const Point3D &point) const
{
    if(point.getX() < pointMin.getX() || pointMax.getX() < point.getX())
        return false;
    if(point.getY() < pointMin.getY() || pointMax.getY() < point.getY())
        return false;
    if(point.getZ() < pointMin.getZ() || pointMax.getZ() < point.getZ())
        return false;
    return true;
}

QDebug operator <<(QDebug debug, const Rect3D &rect)
{
    debug << "Rect3D(" << rect.getPointMin() << "->" << rect.getPointMax() << ")";
    return debug;
}
