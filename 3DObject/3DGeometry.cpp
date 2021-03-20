#include "3DGeometry.h"

bool isNull(const doubli& d) { return -0.000001 < d && d < 0.000001; }
doubli roundNull(const doubli& d) { return isNull(d) ? 0 : d; }
doubli round(const doubli& d) { return std::floor(d * 1000000 + 0.5L) / 1000000; }
doubli sqr(const doubli& d) { return d * d; }
radiant degreesToRadians(const doubli& deg) { return fmodl(deg, 360) * doubli(M_PI) / 180; }
doubli radiansToDegrees(const radiant& rad) { return rad * 180 / doubli(M_PI); }
doubli sqrt(const doubli& d) { return std::sqrt(d); }
doubli cos(const radiant& deg) { return std::cos(deg); }
doubli sin(const radiant& deg) { return std::sin(deg); }
doubli tan(const radiant& deg) { return std::tan(deg); }
radiant acos(const doubli& d) { return std::acos(d); }
radiant asin(const doubli& d) { return std::asin(d); }
radiant atan(const doubli& d) { return std::atan(d); }
doubli qIsInf(const doubli& d) { return qIsInf(static_cast<double>(d)); }
doubli max(const doubli& a, const doubli& b) { return a > b ? a : b; }
doubli max(const doubli& a, const doubli& b, const doubli& c) { return a > b ? (a > c ? a : c) : (b > c ? b : c); }
doubli min(const doubli& a, const doubli& b) { return a < b ? a : b; }
doubli min(const doubli& a, const doubli& b, const doubli& c) { return a < b ? (a < c ? a : c) : (b < c ? b : c); }
QDebug operator <<(QDebug debug, const doubli& d) { debug << static_cast<double>(d); return debug; }


Point3D::Point3D() { x = 0; y = 0; z = 0; }
Point3D::Point3D(doubli x, doubli y, doubli z)
{
    this->x = round(x);
    this->y = round(y);
    this->z = round(z);
    defined = true;
}
Point3D::Point3D(const Point3D& point)
{
    x = point.x;
    y = point.y;
    z = point.z;
    defined = point.defined;
}
Point3D* Point3D::operator =(const Point3D& point)
{
    x = point.x;
    y = point.y;
    z = point.z;
    defined = point.defined;
    return this;
}

bool Point3D::operator <(const Point3D& point) const
{
    //le plus petit = la plus proche
    if (z == point.z)
        return distanceAxeZ() < point.distanceAxeZ();//si plus proche axe Z => plus proche
    else
        return z < point.z;//si plus proche Z alors plus proche
}
bool Point3D::operator >(const Point3D& point) const
{
    //le plus grand = la plus loins
    if (z == point.z)
        return distanceAxeZ() > point.distanceAxeZ();//si plus loins axe Z => plus loins
    else
        return z > point.z;//si plus loins Z alors plus loins
}
bool Point3D::operator ==(const Point3D& point) const
{
    return (x == point.x) && (y == point.y) && (z == point.z);
}

bool Point3D::operator !=(const Point3D &point) const
{
    return (x != point.x) || (y != point.y) || (z != point.z);
}

Point3D Point3D::operator +(const Point3D& point) const
{
    return Point3D(x + point.x,
        y + point.y,
        z + point.z);
}
Point3D Point3D::operator -(const Point3D& point) const
{
    return Point3D(x - point.x,
        y - point.y,
        z - point.z);
}
Point3D Point3D::operator *(const Point3D& point) const
{
    return Point3D(x * point.x,
        y * point.y,
        z * point.z);
}
Point3D Point3D::operator *(doubli n) const { return Point3D(x * n, y * n, z * n); }
Point3D Point3D::operator /(const Point3D& point) const
{
    return Point3D(x / point.x,
        y / point.y,
        z / point.z);
}
Point3D Point3D::operator /(doubli n) const { return Point3D(x / n, y / n, z / n); }

doubli Point3D::distance(const Point3D& pA, const Point3D& pB)
{
    return sqrt(sqr(pB.x - pA.x) +
        sqr(pB.y - pA.y) +
        sqr(pB.z - pA.z));
}
doubli Point3D::distanceMax(const Point3D& pA, const Point3D& pB)
{
    return max(abs(pB.x - pA.x), abs(pB.y - pA.y), abs(pB.z - pA.z));
    //on prend le plus grand (ça forme un carré)
}

QDebug operator <<(QDebug debug, const Point3D& point)
{
    debug << "Point3D(" << point.x << "," << point.y << "," << point.z << ")";
    return debug;
}
Point3D qFloor(const Point3D& point)
{
    return Point3D(std::floor(point.x), std::floor(point.y), std::floor(point.z));
}
Point3D qCeil(const Point3D& point)
{
    return Point3D(std::ceil(point.x), std::ceil(point.y), std::ceil(point.z));
}






Pos3D::Pos3D() : Point3D(0, 0, 0) { rX = 0; rZ = 0; }
Pos3D::Pos3D(doubli x, doubli y, doubli z, radiant rX, radiant rZ) : Point3D(x, y, z)
{
    this->rX = rX; this->rZ = rZ;
}
Pos3D::Pos3D(const Point3D& point, radiant rX, radiant rZ) : Point3D(point)
{
    this->rX = rX; this->rZ = rZ;
}
Pos3D::Pos3D(const Pos3D& pos) : Point3D(pos) { rX = pos.rX; rZ = pos.rZ; }

Pos3D* Pos3D::operator=(const Pos3D& pos)
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
    addX(cos(rX + rot) * speed);
    addY(sin(rX + rot) * speed);
}

Point3D Pos3D::pointFromRot(doubli d, radiant rX, radiant rZ)
{
    return Point3D(cos(rZ) * cos(rX),
        cos(rZ) * sin(rX),
        sin(rZ) * d) * d;
}
Point3D Pos3D::getNextPointRelatif() const
{
    //on s'avance de 1 t
    //équivalent à pointFromRot(1, rX, rZ);
    return Point3D(cos(rZ) * cos(rX), cos(rZ) * sin(rX), sin(rZ));
}
Point3D Pos3D::getNextPoint() const
{
    return Point3D(cos(rZ) * cos(rX) + getX(),
        cos(rZ) * sin(rX) + getY(),
        sin(rZ) + getZ());
    //opti mais == getNextPointRelatif() + getPoint()
}

Pos3D Pos3D::getChildRot(radiant rXRelatif, radiant rZRelatif) const
{
    //changeRef de childNext
    doubli crZR = cos(rZRelatif),
        crZ = cos(rZ), crX = cos(rX),
        srZ = sin(rZ), srX = sin(rX);
    doubli x1 = crZR * cos(rXRelatif), y1 = crZR * sin(rXRelatif), z1 = sin(rZRelatif);//z1==srZR
    doubli x2 = x1 * crZ - z1 * srZ, z2 = x1 * srZ + z1 * crZ;//y2==y1
    doubli x3 = x2 * crX - y1 * srX, y3 = x2 * srX + y1 * crX;//z3==z2
    radiant rX = atan(y3 / x3);
    if (x3 < 0) {//sinon on ne voit que tout ce qui est en x3>=0 (rotation de 180° du reste)
        rX += M_PI;//atan retourne -89.5 alors qu'il faut 90.5 (mais ça reste la meme chose)
        //pas besoin de différentier y car on est à k360° de diff
    }
    return Pos3D(getPoint(), rX, asin(z2));
}

Point3D Pos3D::changeRef(const Point3D& point) const { return rotation(point + getPoint(), rX, rZ); }

bool Pos3D::operator ==(const Pos3D& pos) const
{
    return Point3D::operator ==(pos) && rX == pos.rX && rZ == pos.rZ;
}

bool Pos3D::operator !=(const Pos3D &pos) const
{
    return Point3D::operator !=(pos) || rX != pos.rX || rZ != pos.rZ;
}

Point3D Pos3D::rotation(Point3D point, radiant rX, radiant rZ)
{
    //inversé par rapport aux autres progs (entre y et z et entre 1ere et 2eme ligne)
    doubli sR = sin(rZ), cR = cos(rZ);
    point = Point3D(rotation1(point.getX(), point.getZ(), sR, cR), point.getY(), rotation2(point.getX(), point.getZ(), sR, cR));
    sR = sin(rX); cR = cos(rX);
    return Point3D(rotation1(point.getX(), point.getY(), sR, cR), rotation2(point.getX(), point.getY(), sR, cR), point.getZ());
}
doubli Pos3D::rotation1(doubli x, doubli y, doubli sR, doubli cR) { return x * cR - y * sR; }
doubli Pos3D::rotation2(doubli x, doubli y, doubli sR, doubli cR) { return x * sR + y * cR; }
Pos3D Pos3D::getRotAsVect(const Point3D& p1, const Point3D& p2)
{
    radiant rZ = asin((p2.getZ() - p1.getZ()) / Point3D::distance(p1, p2));//formule getNextPos z inversée
    radiant rX = atan((p2.getY() - p1.getY()) / (p2.getX() - p1.getX()));
    if (p2.getX() - p1.getX() < 0) {
        if (p2.getY() - p1.getY() >= 0)
            rX += M_PI;//atan retourne -89.5 alors qu'il faut 90.5 (mais ça reste la meme chose)
        else
            rX -= M_PI;
    }
    return Pos3D(p1, rX, rZ);
}

Pos3D Pos3D::getRotAsPoint(const Point3D& p)
{
    //opti mais == getRotAsVect(Point3D(0,0,0),p)
    doubli d = Point3D::distance(Point3D(0, 0, 0), p);
    radiant rZ = asin((p.getZ()) / d);//formule getNextPos z inversée
    radiant rX = atan((p.getY()) / (p.getX()));
    if (p.getX() < 0) {
        if (p.getY() >= 0)
            rX += M_PI;//atan retourne -89.5 alors qu'il faut 90.5 (mais ça reste la meme chose)
        else
            rX -= M_PI;
    }
    return Pos3D(0, 0, 0, rX, rZ);
}


QDebug operator <<(QDebug debug, const Pos3D& pos)
{
    debug << "Pos3D(" << pos.getX() << "," << pos.getY() << "," << pos.getZ()
        << ";" << pos.getRX() << "," << pos.getRZ() << ")";
    return debug;
}



Size3D::Size3D(doubli dX, doubli dY, doubli dZ)
{
    this->dX = dX; this->dY = dY; this->dZ = dZ;
}
Size3D::Size3D(const Size3D& size)
{
    dX = size.dX; dY = size.dY; dZ = size.dZ;
}
Size3D::Size3D(const Point3D& pA, const Point3D& pB)
{
    dX = pB.getX() - pA.getX();
    dY = pB.getY() - pA.getY();
    dZ = pB.getZ() - pA.getZ();
}
Point3D operator -(const Point3D& p, const Size3D& s)
{
    return Point3D(p.getX() - s.getDX(), p.getY() - s.getDY(), p.getZ() - s.getDZ());
}
Point3D operator +(const Point3D& p, const Size3D& s)
{
    return Point3D(p.getX() + s.getDX(), p.getY() + s.getDY(), p.getZ() + s.getDZ());
}



HRect3D::HRect3D() {}
HRect3D::HRect3D(const Point3D& pointA, const Point3D& pointB)
{
    pointMin = Point3D(min(pointA.getX(), pointB.getX()),
        min(pointA.getY(), pointB.getY()),
        min(pointA.getZ(), pointB.getZ()));
    pointMax = Point3D(max(pointA.getX(), pointB.getX()),
        max(pointA.getY(), pointB.getY()),
        max(pointA.getZ(), pointB.getZ()));
}
HRect3D::HRect3D(const Point3D& pointA, const Size3D& size)
{
    pointMin = pointA; pointMax = pointA + size;
}
HRect3D::HRect3D(const HRect3D& rect)
{
    pointMin = rect.pointMin; pointMax = rect.pointMax;
}

void HRect3D::scale(doubli scale)
{
    Size3D demisizeScaled = getSize() / 2 * scale;
    const Point3D& middle = getMiddle();
    pointMin = middle - demisizeScaled;
    pointMax = middle + demisizeScaled;
}

HRect3D* HRect3D::operator =(const HRect3D& rect)
{
    pointMin = rect.pointMin; pointMax = rect.pointMax;
    return this;
}
bool HRect3D::operator ==(const HRect3D& rect) const
{
    return pointMin == rect.pointMin && pointMax == rect.pointMax;
}

bool HRect3D::contains(const Point3D& point) const
{
    if (point.getX() < pointMin.getX() || pointMax.getX() < point.getX())
        return false;
    if (point.getY() < pointMin.getY() || pointMax.getY() < point.getY())
        return false;
    if (point.getZ() < pointMin.getZ() || pointMax.getZ() < point.getZ())
        return false;
    return true;
}

QDebug operator <<(QDebug debug, const HRect3D& rect)
{
    debug << "HRect3D(" << rect.pointMin << "->" << rect.pointMax << ")";
    return debug;
}



Rect3D::Rect3D() : HRect3D() {}
Rect3D::Rect3D(const Point3D& pointA, const Point3D& pointB, const Point3D& pointC) : HRect3D(pointA, pointC)
{
    this->pointB = pointB;
    calcMinMax();
}
Rect3D::Rect3D(const Point3D& pointA, const Point3D& pointB, const Size3D& size) : HRect3D(pointA, size)
{
    this->pointB = pointB;
    calcMinMax();
}
Rect3D::Rect3D(const Rect3D& rect) : HRect3D(rect)
{
    pointB = rect.pointB;
    pointMin = rect.pointMin;
    pointMax = rect.pointMax;
}

void Rect3D::scale(doubli scale)
{
    HRect3D::scale(scale);
    Size3D thirdSize = Size3D(getMiddle(), pointB) * scale;
    pointB = pointB + thirdSize;
    calcMinMax();
}

Rect3D* Rect3D::operator=(const Rect3D& rect)
{
    HRect3D::operator=(rect);
    pointB = rect.pointB;
    pointMin = rect.pointMin;
    pointMax = rect.pointMax;
    return this;
}

bool Rect3D::operator==(const Rect3D& rect) const
{
    return HRect3D::operator==(rect) && pointB == rect.pointB;
}

bool Rect3D::contains(const Point3D& point) const
{
    if (point.getX() < pointMin.getX() || pointMax.getX() < point.getX())
        return false;
    if (point.getY() < pointMin.getY() || pointMax.getY() < point.getY())
        return false;
    if (point.getZ() < pointMin.getZ() || pointMax.getZ() < point.getZ())
        return false;
    return true;
}

QDebug operator <<(QDebug debug, const Rect3D& rect)
{
    debug << "Rect3D(" << rect.getPointMin() << "->" << rect.getPointMax() << ")";
    return debug;
}

void Rect3D::calcMinMax()
{
    pointMax = Point3D(max(HRect3D::getPointMax().getX(), pointB.getX()),
        max(HRect3D::getPointMax().getY(), pointB.getY()),
        max(HRect3D::getPointMax().getZ(), pointB.getZ()));
    pointMin = Point3D(min(HRect3D::getPointMin().getX(), pointB.getX(), pointC.getX()),
        min(HRect3D::getPointMin().getY(), pointB.getY()),
        min(HRect3D::getPointMin().getZ(), pointB.getZ()));
}




Plan::Plan() {}
Plan::Plan(const Point3D& pA, const Point3D& pB, const Point3D& pC)
{
    this->pA = pA;
    this->pB = pB;
    this->pC = pC;
    calcEquation();
}
Plan::Plan(const HRect3D& rect)
{
    this->pA = rect.getPointMin();
    //les points intermédiaires (B et C) sont à mis distance de A et de D sur Z
    //TODO: sauf si rotation...
    this->pB = Point3D(rect.getPointMin().getX(),
        (rect.getPointMin().getY() + rect.getPointMax().getY()) / 2,
        (rect.getPointMin().getZ() + rect.getPointMax().getZ()) / 2);
    this->pC = Point3D((rect.getPointMin().getX() + rect.getPointMax().getX()) / 2,
        rect.getPointMin().getY(),
        (rect.getPointMin().getZ() + rect.getPointMax().getZ()) / 2);
    calcEquation();
}
Plan::Plan(const Plan& plan)
{
    pA = plan.pA; pB = plan.pB; pC = plan.pC;
    a = plan.a; b = plan.b; c = plan.c; d = plan.d;
}

Plan* Plan::operator =(const Plan& plan)
{
    pA = plan.pA; pB = plan.pB; pC = plan.pC;
    a = plan.a; b = plan.b; c = plan.c; d = plan.d;
    return this;
}

bool Plan::paralleleDroite(const Size3D& vect) const
{
    //voir #Orthogonal
    doubli scalaireAvecVectNormal = a * vect.getDX() +
        b * vect.getDY() +
        c * vect.getDZ();
    return isNull(scalaireAvecVectNormal);
}

Point3D Plan::intersection(const Point3D& pA, const Point3D& pB) const
{
    //Voir #Intersection
    Size3D ABs(pA, pB);
    if (paralleleDroite(ABs))
        return Point3D();
    doubli t = -(a * pA.getX() + b * pA.getY() + c * pA.getZ() + d)
        / (a * ABs.getDX() + b * ABs.getDY() + c * ABs.getDZ());
    if (t < 0)//derriere
        return Point3D();
    return pA + ABs * t;
}

QPointF Plan::projeteOrtho(const Point3D& pA) const
{
    //donne un point de position relative au plan (à la texture...)
    //un vecteur normal: m(a, b, c) donne la direction du vecteur MM'
    //un vecteur normal donne la rotation du Plan: sin(rY) = b / distance, sin(rX) = z / distance
    doubli distance = sqrt(a * a + b * b + c * c);
    Point3D pR = Pos3D(this->pA, asin(b / distance), asin(c / distance)).changeRef(pA);
    //on prend direct pA car si c'est pas un point du plan alors pR est son projeté dans l'autreref
    //(si x != 0 alors pA n'est pas sur le plan)


    return QPointF(pR.getY(), pR.getZ());
    //ne prend pas en compte la rotation... ! (relative au point A)
}

void Plan::calcEquation()
{
    // same as Size but way more efficient
    doubli xAB = roundNull(pB.getX() - pA.getX()),
        yAB = roundNull(pB.getY() - pA.getY()),
        zAB = roundNull(pB.getZ() - pA.getZ()),
        xAC = roundNull(pC.getX() - pA.getX()),
        yAC = roundNull(pC.getY() - pA.getY()),
        zAC = roundNull(pC.getZ() - pA.getZ());
    //voir #EquationPlan
    a = 0; b = 0; c = 0; d = 0;
    if ((xAB == 0 && yAB == 0 && zAB == 0) || (xAC == 0 && yAC == 0 && zAC == 0)) {
        //qDebug() << "ERROR Plan vecteur nul";
        return;
    }
    doubli nOfb = yAB / yAC, nOfc = zAB / zAC;
    if (zAB == 0 && zAC == 0) nOfc = 0;
    if (yAB == 0 && yAC == 0) nOfb = 0;
    //avec nOfb le coef qui permet à b d'être nul et nOfc pour qvoir c nul

    //bug équation du plan avec
    //A(0,0,0) B(0,2,0) C(1,0,0) => AB(0,2,0) AC(1,0,0)
    //P : z=0 => c=1 et le reste=0
    //si les 2 vecteurs sont sur le plan déssiné par 2 axes
    if (zAB == 0 && zAC == 0) {
        c = 1;//z - d = 0
    }
    else if (yAB == 0 && yAC == 0) {
        b = 1;//y - d = 0
    }
    else if (xAB == 0 && xAC == 0) {
        a = 1;//x - d = 0
    }
    //si AB est un vecteur dir des axes
    else if (xAB == 0 && yAB == 0) {
        //zAC ne sert à rien puisqu'il dessine une droite // à AB
        a = zAB * yAC;
        b = -zAB * xAC;
    }
    else if (xAB == 0 && zAB == 0) {
        a = yAB * zAC;
        c = -yAB * xAC;
    }
    else if (yAB == 0 && zAB == 0) {
        b = xAB * zAC;
        c = -xAB * yAC;
    }
    //si AC est un vecteur directeur d'un des axes
    else if (xAC == 0 && yAC == 0) {
        a = zAC * yAB;
        b = -zAC * xAB;
    }
    else if (xAC == 0 && zAC == 0) {
        a = yAC * zAB;
        c = -yAC * xAB;
    }
    else if (yAC == 0 && zAC == 0) {
        b = xAC * zAB;
        c = -xAC * yAB;
    }
    //sinon
    else {
        a = 1;
        if (yAC == 0)
            c = (xAC)*a / (-zAC);//nOfb == inf donc on retire et fait *0 pr l'autre
        else
            c = (xAC * nOfb - xAB) * a / (zAB - zAC * nOfb);
        if (zAC == 0)
            b = (xAC)*a / (-yAC);//nOfc == inf donc on retire et fait *0 pr l'autre
        else
            b = (xAC * nOfc - xAB) * a / (yAB - yAC * nOfc);
    }
    d = roundNull(-a * pA.getX() - b * pA.getY() - c * pA.getZ());
    //si il y a 2 x, y ou z ==0 pr chaque vecteur alors vecteurs colinéaires => pas de plan
}
