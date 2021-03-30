#include "Geometry3D.h"

Size3D::Size3D(doubli dX, doubli dY, doubli dZ)
{
    this->dX = dX;
    this->dY = dY;
    this->dZ = dZ;
}
Size3D::Size3D(const Size3D &size)
{
    dX = size.dX;
    dY = size.dY;
    dZ = size.dZ;
}
Size3D::Size3D(const Point3D &pA, const Point3D &pB)
{
    dX = pB.x() - pA.x();
    dY = pB.y() - pA.y();
    dZ = pB.z() - pA.z();
}

Point3D operator-(const Point3D &p, const Size3D &s)
{
    return Point3D(p.x() - s.dX, p.y() - s.dY, p.z() - s.dZ);
}
Point3D operator+(const Point3D &p, const Size3D &s)
{
    return Point3D(p.x() + s.dX, p.y() + s.dY, p.z() + s.dZ);
}

HRect3D::HRect3D() {}
HRect3D::HRect3D(const Point3D &pointA, const Point3D &pointB)
{
    pointMin = Point3D(min(pointA.x(), pointB.x()), min(pointA.y(), pointB.y()),
                       min(pointA.z(), pointB.z()));
    pointMax = Point3D(max(pointA.x(), pointB.x()), max(pointA.y(), pointB.y()),
                       max(pointA.z(), pointB.z()));
}
HRect3D::HRect3D(const Point3D &pointA, const Size3D &size)
{
    pointMin = pointA;
    pointMax = pointA + size;
}
HRect3D::HRect3D(const HRect3D &rect)
{
    pointMin = rect.pointMin;
    pointMax = rect.pointMax;
}

void HRect3D::scale(doubli scale)
{
    Size3D demisizeScaled = getSize() / 2 * scale;
    const Point3D &middle = getMiddle();
    pointMin = middle - demisizeScaled;
    pointMax = middle + demisizeScaled;
}

HRect3D *HRect3D::operator=(const HRect3D &rect)
{
    pointMin = rect.pointMin;
    pointMax = rect.pointMax;
    return this;
}
bool HRect3D::operator==(const HRect3D &rect) const
{
    return pointMin == rect.pointMin && pointMax == rect.pointMax;
}

bool HRect3D::contains(const Point3D &point) const
{
    if (point.x() < pointMin.x() || pointMax.x() < point.x()) return false;
    if (point.y() < pointMin.y() || pointMax.y() < point.y()) return false;
    if (point.z() < pointMin.z() || pointMax.z() < point.z()) return false;
    return true;
}

QDebug operator<<(QDebug debug, const HRect3D &rect)
{
    debug << "HRect3D(" << rect.pointMin << "->" << rect.pointMax << ")";
    return debug;
}

Rect3D::Rect3D() : HRect3D() {}
Rect3D::Rect3D(const Point3D &pointA, const Point3D &pointB, const Point3D &pointC)
    : HRect3D(pointA, pointC)
{
    this->pointB = pointB;
    calcMinMax();
}
Rect3D::Rect3D(const Point3D &pointA, const Point3D &pointB, const Size3D &size)
    : HRect3D(pointA, size)
{
    this->pointB = pointB;
    calcMinMax();
}
Rect3D::Rect3D(const Rect3D &rect) : HRect3D(rect)
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

Rect3D *Rect3D::operator=(const Rect3D &rect)
{
    HRect3D::operator=(rect);
    pointB = rect.pointB;
    pointMin = rect.pointMin;
    pointMax = rect.pointMax;
    return this;
}

bool Rect3D::operator==(const Rect3D &rect) const
{
    return HRect3D::operator==(rect) && pointB == rect.pointB;
}

bool Rect3D::contains(const Point3D &point) const
{
    if (point.x() < pointMin.x() || pointMax.x() < point.x()) return false;
    if (point.y() < pointMin.y() || pointMax.y() < point.y()) return false;
    if (point.z() < pointMin.z() || pointMax.z() < point.z()) return false;
    return true;
}

QDebug operator<<(QDebug debug, const Rect3D &rect)
{
    debug << "Rect3D(" << rect.getPointMin() << "->" << rect.getPointMax() << ")";
    return debug;
}

void Rect3D::calcMinMax()
{
    pointMax = Point3D(max(HRect3D::getPointMax().x(), pointB.x()),
                       max(HRect3D::getPointMax().y(), pointB.y()),
                       max(HRect3D::getPointMax().z(), pointB.z()));
    pointMin = Point3D(min(HRect3D::getPointMin().x(), pointB.x(), pointC.x()),
                       min(HRect3D::getPointMin().y(), pointB.y()),
                       min(HRect3D::getPointMin().z(), pointB.z()));
}

Plan::Plan() {}
Plan::Plan(const Point3D &pA, const Point3D &pB, const Point3D &pC)
{
    this->pA = pA;
    calcEquation(pB, pC);
}
Plan::Plan(const HRect3D &rect)
{
    this->pA = rect.getPointMin();
    // les points intermédiaires (B et C) sont à mis distance de A et de D sur Z
    // TODO: sauf si rotation...
    const Point3D pB(rect.getPointMin().x(), (rect.getPointMin().y() + rect.getPointMax().y()) / 2,
                     (rect.getPointMin().z() + rect.getPointMax().z()) / 2);
    const Point3D pC((rect.getPointMin().x() + rect.getPointMax().x()) / 2, rect.getPointMin().y(),
                     (rect.getPointMin().z() + rect.getPointMax().z()) / 2);
    calcEquation(pB, pC);
}
Plan::Plan(const Plan &plan)
{
    pA = plan.pA;
    a = plan.a;
    b = plan.b;
    c = plan.c;
    d = plan.d;
}

Plan *Plan::operator=(const Plan &plan)
{
    pA = plan.pA;
    a = plan.a;
    b = plan.b;
    c = plan.c;
    d = plan.d;
    return this;
}

bool Plan::paralleleDroite(const Size3D &vect) const
{
    // voir #Orthogonal
    doubli scalaireAvecVectNormal = a * vect.getDX() + b * vect.getDY() + c * vect.getDZ();
    return isNull(scalaireAvecVectNormal);
}

Point3DCancelable Plan::intersection(const Point3D &pA, const Point3D &pB) const
{
    // Voir #Intersection
    Size3D ABs(pA, pB);
    if (paralleleDroite(ABs)) return Point3DCancelable();
    doubli t = -(a * pA.x() + b * pA.y() + c * pA.z() + d)
               / (a * ABs.getDX() + b * ABs.getDY() + c * ABs.getDZ());
    if (t < 0) // derriere
        return Point3DCancelable();
    return Point3DCancelable(pA + ABs * t);
}

QPointF Plan::projeteOrtho(const Point3D &pA) const
{
    // donne un point de position relative au plan (à la texture...)
    // un vecteur normal: m(a, b, c) donne la direction du vecteur MM'
    // un vecteur normal donne la rotation du Plan: sin(rY) = b / distance, sin(rX) = z / distance
    // calcEquation donne des paramètres unitaires donc distance == 1

    Point3D pR = Pos3D(this->pA, 0, 0).changeRef(pA, b, c);
    // on prend direct pA car si c'est pas un point du plan alors pR est son projeté dans l'autreref
    //(si x != 0 alors pA n'est pas sur le plan)

    return QPointF(pR.y(), pR.z());
    // ne prend pas en compte la rotation... ! (relative au point A)
}

Point3D Plan::projeteOrtho3D(const Point3D &pA) const
{
    doubli dA = -(a * pA.x() + b * pA.y() + c * pA.z());
    doubli k = dA - d; // /(a²+b²+c²) soit /1
    return pA + Point3D(k * a, k * b, k * c);
}

bool Plan::containsPoint(const Point3D &point) const
{
    return isNull(a * point.x() + b * point.y() + c * point.z() + d);
}

void Plan::calcEquation(const Point3D &pB, const Point3D &pC)
{
    // same as Size but way more efficient
    doubli xAB = (pB.x() - pA.x()), yAB = (pB.y() - pA.y()), zAB = (pB.z() - pA.z()),
           xAC = (pC.x() - pA.x()), yAC = (pC.y() - pA.y()), zAC = (pC.z() - pA.z());
    // voir #EquationPlan
    a = 0;
    b = 0;
    c = 0;
    d = 0;
    if ((xAB == 0 && yAB == 0 && zAB == 0) || (xAC == 0 && yAC == 0 && zAC == 0)) {
        // qDebug() << "ERROR Plan vecteur nul";
        return;
    }
    doubli nOfb = yAB / yAC, nOfc = zAB / zAC;
    if (zAB == 0 && zAC == 0) nOfc = 0;
    if (yAB == 0 && yAC == 0) nOfb = 0;
    // avec nOfb le coef qui permet à b d'être nul et nOfc pour qvoir c nul

    // bug équation du plan avec
    // A(0,0,0) B(0,2,0) C(1,0,0) => AB(0,2,0) AC(1,0,0)
    // P : z=0 => c=1 et le reste=0
    // si les 2 vecteurs sont sur le plan déssiné par 2 axes
    if (zAB == 0 && zAC == 0) {
        c = 1; // z - d = 0
    } else if (yAB == 0 && yAC == 0) {
        b = 1; // y - d = 0
    } else if (xAB == 0 && xAC == 0) {
        a = 1; // x - d = 0
    }
    // si AB est un vecteur dir des axes
    else if (xAB == 0 && yAB == 0) {
        // zAC ne sert à rien puisqu'il dessine une droite // à AB
        a = zAB * yAC;
        b = -zAB * xAC;
    } else if (xAB == 0 && zAB == 0) {
        a = yAB * zAC;
        c = -yAB * xAC;
    } else if (yAB == 0 && zAB == 0) {
        b = xAB * zAC;
        c = -xAB * yAC;
    }
    // si AC est un vecteur directeur d'un des axes
    else if (xAC == 0 && yAC == 0) {
        a = zAC * yAB;
        b = -zAC * xAB;
    } else if (xAC == 0 && zAC == 0) {
        a = yAC * zAB;
        c = -yAC * xAB;
    } else if (yAC == 0 && zAC == 0) {
        b = xAC * zAB;
        c = -xAC * yAB;
    }
    // sinon
    else {
        a = 1;
        if (yAC == 0)
            c = (xAC)*a / (-zAC); // nOfb == inf donc on retire et fait *0 pr l'autre
        else
            c = (xAC * nOfb - xAB) * a / (zAB - zAC * nOfb);
        if (zAC == 0)
            b = (xAC)*a / (-yAC); // nOfc == inf donc on retire et fait *0 pr l'autre
        else
            b = (xAC * nOfc - xAB) * a / (yAB - yAC * nOfc);
    }

    // unitaire
    doubli dist = sqrt(a * a + b * b + c * c);
    if (dist != 0) {
        a /= dist;
        b /= dist;
        c /= dist;
    }

    d = (-a * pA.x() - b * pA.y() - c * pA.z());
    // si il y a 2 x, y ou z ==0 pr chaque vecteur alors vecteurs colinéaires => pas de plan
}
