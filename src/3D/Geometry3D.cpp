#include "Geometry3D.h"

void HRect3D::scale(const doubli &scale)
{
    const Size3D demisizeScaled = getSize() / 2 * scale;
    const Point3D middle = getMiddle();
    pMin = middle - demisizeScaled;
    pMax = middle + demisizeScaled;
}

bool HRect3D::operator==(const HRect3D &rect) const
{
    return pMin == rect.pMin && pMax == rect.pMax;
}

bool HRect3D::containsLine(const Point3D &pA, const Point3D &pB) const
{
    // est ce que la droite peut atteindre le block ?
    // voir # Calculs pour HRect3D::containsLine
    // https://github.com/Jiogo18/RayTracing/blob/master/tests/equations.txt
    const doubli a = pB.x() - pA.x(), b = pB.y() - pA.y(), c = pB.z() - pA.z();

    const doubli t1X = (pMin.x() - pA.x()) / a, t2X = (pMax.x() - pA.x()) / a;
    const doubli t1Y = (pMin.y() - pA.y()) / b, t2Y = (pMax.y() - pA.y()) / b;
    const doubli t1Z = (pMin.z() - pA.z()) / c, t2Z = (pMax.z() - pA.z()) / c;

    const doubli tMaxX = maths3D::max(t1X, t2X), tMinX = maths3D::min(t1X, t2X);
    const doubli tMaxY = maths3D::max(t1Y, t2Y), tMinY = maths3D::min(t1Y, t2Y);
    const doubli tMaxZ = maths3D::max(t1Z, t2Z), tMinZ = maths3D::min(t1Z, t2Z);

    const doubli tMin = maths3D::max(tMinX, tMinY, tMinZ);
    const doubli tMax = maths3D::min(tMaxX, tMaxY, tMaxZ);
    return tMin <= tMax;
    // t \in [tMin;tMax] // TODO: calculer avec les t ! ça sera plus simple ?
    // oui, surtout pour savoir la quantitée de matière traversée
    // ça sera plus simple pour les HRect mais pour les autres formes..?
}

QDebug operator<<(QDebug debug, const HRect3D &rect)
{
    debug.nospace() << "HRect3D(" << rect.pMin << "->" << rect.pMax << ")";
    return debug;
}

void Rect3D::scale(const doubli &scale)
{
    HRect3D::scale(scale);
    Size3D size(getMiddle(), pB);
    pB = pB + size * scale;
    calcMinMax();
}

Rect3D *Rect3D::operator=(const Rect3D &rect)
{
    HRect3D::operator=(rect);
    pB = rect.pB;
    pMin = rect.pMin;
    pMax = rect.pMax;
    return this;
}

bool Rect3D::operator==(const Rect3D &rect) const
{
    return HRect3D::operator==(rect) && pB == rect.pB;
}

bool Rect3D::contains(const Point3D &point) const
{
    return pMin.x() <= point.x() && point.x() <= pMax.x()
           && pMin.y() <= point.y() && point.y() <= pMax.y()
           && pMin.z() <= point.z() && point.z() <= pMax.z();
}

QDebug operator<<(QDebug debug, const Rect3D &rect)
{
    debug.nospace() << "Rect3D(" << rect.getPointMin() << "->" << rect.getPointMax() << ")";
    return debug;
}

Plan::Plan(const HRect3D &r) : pA(r.getPointMin())
{
    // les points intermédiaires (B et C) sont à mis distance de A et de D sur Z
    // TODO: sauf si rotation...
    const Point3D pB(r.getPointMin().x(), r.getMiddle().y(), r.getMiddle().z());
    const Point3D pC(r.getMiddle().x(), r.getPointMin().y(), r.getMiddle().z());
    calcEquation(pB, pC);
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
    doubli scalaireAvecVectNormal = a * vect.dX() + b * vect.dY() + c * vect.dZ();
    return isNull(scalaireAvecVectNormal);
}

Point3DCancelable Plan::intersection(const Point3D &pA, const Point3D &pB) const
{
    // Voir #Intersection
    Size3D ABs(pA, pB);
    if (paralleleDroite(ABs)) return Point3DCancelable();
    doubli t = -(a * pA.x() + b * pA.y() + c * pA.z() + d)
               / (a * ABs.dX() + b * ABs.dY() + c * ABs.dZ());
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
        //qDebug() << "ERROR Plan vecteur nul";
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
