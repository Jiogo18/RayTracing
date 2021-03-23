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

radian Transfo3D::refract(const radian& r, float indiceRefrac)
{
    // sin(angle in) / sin(angle out) = i out / i in
    // sin(angle out) = sin(angle in) * i in / i out = sin(angle in) * speed out / speed in
    radian r4 = asin(sin(r) * indiceRefrac);
    if (isnanf(r4)) return signOf(r) * M_PI_2;
    else return r4;
}

Pos3D Transfo3D::refractRot(const Pos3D& posOrigin, const Pos3D& pos, float indiceRefrac)
{
    if (indiceRefrac == 1) return pos;// shortcut
    //TODO :
    return pos;
    radian rXRel = modRad(pos.getRX() - posOrigin.getRX());
    radian rX4 = posOrigin.getRX() - (refract(rXRel, indiceRefrac) * cos(posOrigin.getRZ()) - abs(sin(posOrigin.getRZ())) * rXRel);

    radian rZRel = modRad(pos.getRZ() - posOrigin.getRZ());
    radian rZ4 = posOrigin.getRZ() + M_PI - refract(rZRel, indiceRefrac);


    return Pos3D(pos, M_PI - rX4, rZ4);
}
