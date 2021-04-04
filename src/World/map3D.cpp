#include "map3D.h"

map3D::map3D() : QObject()
{
    world = new World();
    client = new Entity(Pos3D::fromDegree(-0.5L, 1.5L, 1.5L, -30, -20), ENTITY::Type::ghost);
    world->addEntity(client);
    //client->moveTo(Pos3D::fromDegree(1.8L, 2, 1.9L, -140, -30));//belle vue
    //client->moveTo(Pos3D::fromDegree(1.5L, 1.1L, 0.9L, -140, -30));//vue green_glass

    //client->moveTo(Pos3D::fromDegree(0.5L, 1.5, 2.5L, -90, -30));//vue miroir verre blanc
    //client->moveTo(Pos3D::fromDegree(1.2L, 0.5, 2.5L, -180, -30));//vue miroir verre vert
    //client->moveTo(Pos3D::fromDegree(0.5L, 4, 1.2L, -90, -10));//vue miroir glowstone
    client->moveTo(Pos3D::fromDegree(0.5, 3, 0.9L, -70, -10)); //belle vue mirroir
    //client->moveTo(Pos3D::fromDegree(0.5, 1.5, 2.5, 0, -90));//verre au dessus

    world->addSolid(new Cube(Pos3D(0, 0, 0, 0, 0), SOLID::Material::oak_log));
    world->addSolid(new Cube(Pos3D(0, 1, 0, 0, 0), SOLID::Material::birch_log));
    world->addSolid(new Cube(Pos3D(0, 0, 1, 0, 0), SOLID::Material::stone));
    world->addSolid(new Cube(Pos3D(0, 1, 1, 0, 0), SOLID::Material::glass));
    world->addSolid(new Cube(Pos3D(1, 0, 0, 0, 0), SOLID::Material::green_glass));
    world->addSolid(new HalfCube(Pos3D(0, 2, -0.25, 0, 0), SOLID::Material::glowstone));
    world->addSolid(new Cube(Pos3D(0, 0, 2, 0, 0), SOLID::Material::mirror));

    //mur de miroir entouré de stone
    fillCube(Point3D(2, 0, 0), Point3D(2, 2, 3), SOLID::Material::stone);
    fillCube(Point3D(2, 1, 0), Point3D(2, 1, 3), SOLID::Material::mirror);

    world->addSolid(new Cube(Pos3D(0, 3, 0, 0, 0), SOLID::Material::mirror));
    world->addSolid(new Cube(Pos3D(1, 3, 0, 0, 0), SOLID::Material::watter));
}

map3D::~map3D()
{
    if (world != nullptr) delete world;
    //client est delete par le world donc c'est bon
}

int map3D::fillCube(const Point3D &posMin, const Point3D &posMax, SOLID::Material blockMaterial)
{
    HRect3D rect(posMin, posMax);

    int blockPlaced = 0;
    for (int x = rect.getPointMin().x(); x <= rect.getPointMax().x(); x++) {
        for (int y = rect.getPointMin().y(); y <= rect.getPointMax().y(); y++) {
            for (int z = rect.getPointMin().z(); z <= rect.getPointMax().z(); z++) {
                Cube *c = new Cube(Pos3D(x, y, z, 0, 0), blockMaterial);
                if (world->addSolid(c))
                    blockPlaced++;
                else
                    delete c;
            }
        }
    }
    return blockPlaced;
}
