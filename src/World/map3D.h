#ifndef MAP3D_H
#define MAP3D_H

#include "../World/World3D.h"
#include "../RayTracing/Transfo.h"
// #include <QGraphicsScene>
// #include <QGraphicsPixmapItem>
#include <thread>
#include <regex>
#include "../utils/QFile.h"

class map3D
{
public:
    map3D();
    ~map3D();
    const Entity *getClient() const { return client; }
    const World *getWorld() const { return world; }

    void moveUp() { client->moveUp(); }
    void moveDown() { client->moveDown(); }
    void moveFront() { client->moveFront(); }
    void moveBack() { client->moveBack(); }
    void moveLeft() { client->moveLeft(); }
    void moveRight() { client->moveRight(); }
    void moveRX(radian rX) { client->moveRX(rX); }
    void moveRZ(radian rZ) { client->moveRZ(rZ); }
    void moveClient(const Pos3D &pos) { client->moveTo(pos); }

    int fillCube(const Point3D &posMin, const Point3D &posMax, SOLID::Material blockMaterial);

    bool load(std::string mapFileName);

    // signals:
    void clientChanged(const Entity *client);

private:
    Entity *client = nullptr;
    World *world = nullptr;
};

#endif // MAP3D_H
