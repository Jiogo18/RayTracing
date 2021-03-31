#include "Transfo.h"

/*
World Transfo3D::changeRef(const World *world, Pos3D pos)
{
    World retour;
    QMapIterator<Timestamp, Block*> iB(world->getBlocks());
    while(iB.hasNext()) {
        iB.next();
        Block *current = new Block(*iB.value());
        current->setPoint(rotation(iB.value()->getPoint() + pos.getPoint(), pos.getRX(), pos.getRY()));
        retour.addBlock(current);
    }
    QMapIterator<Timestamp, Entity*> iE(world->getEntities());
    while(iE.hasNext()) {
        iE.next();
        Entity *current = new Entity(*iE.value());
        current->setPoint(rotation(iE.value()->getPoint() + pos.getPoint(), pos.getRX(), pos.getRY()));
        retour.addBlock(current);
    }
    return *world;
}*/

/*void Transfo3D::addPerspective(World *world, double persp, double zoom)
{

}
Point3D Transfo3D::pointPerspective(Point3D point, double perspective, double zoom)
{
    return point;
}*/

Rot3D Transfo3D::refractRot(const Rot3D &posOrigin, const Rot3D &pos, float indiceRefrac)
{
    if (indiceRefrac == 1) return pos;       // shortcut
    if (indiceRefrac == 0) return posOrigin; // solide

    Point3D Ori = posOrigin.toVector();
    Point3D delta = pos.toVector() - Ori;

    Point3D Mid = Ori + delta / indiceRefrac;

#ifdef NAN_VERIF
    if (Mid.isInf() || Mid.isNan()) {
        qDebug() << "Transfo::refractRot Mid is nan or inf:" << Mid << indiceRefrac;
    }
#endif // NAN_VERIF

    return Rot3D::fromVector(Mid);
}
