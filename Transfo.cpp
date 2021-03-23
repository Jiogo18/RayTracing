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

Rot3D Transfo3D::refractRot(const Rot3D& posOrigin, const Rot3D& pos, float indiceRefrac)
{
    if (indiceRefrac == 1) return pos;// shortcut

    return pos;//TODO: on reste avec un mauvais rX en sortie
    radian RX = posOrigin.getRX(), RZ = posOrigin.getRZ();

    radian rXRel = modRad(pos.getRX() - RX);
    radian rZRel = modRad(pos.getRZ() - RZ);

    //    radian rX4 = RX - (refract(rXRel, indiceRefrac) * cos(RZ) - abs(sin(RZ)) * rXRel);
    //    radian rZ4 = RZ + M_PI - refract(rZRel, indiceRefrac);
    //    radian rX4 = RX + refract(rXRel, indiceRefrac);
    radian rX4 = RX + refract(rXRel, indiceRefrac);
    radian rZ4 = RZ + refract(rZRel, indiceRefrac);
    //    radian rZ4 = RZ + M_PI - refract(rZRel, indiceRefrac);


    return Rot3D(rX4, rZ4);
}
