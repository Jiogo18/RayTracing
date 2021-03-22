#ifndef WORLD3D_H
#define WORLD3D_H

#include "Object3D.h"

class Chunk : private Object
{
public:
    Chunk(Point3D pos);
    ~Chunk() override;
    static const int chunkSize = 8;
    Point3D getPoint() const { return Object::getPoint(); }

    bool addSolid(Solid* block);
    bool haveSolid(const Point3D& pos) const;
    Solid* getSolid(const Point3D& pos) const;
    bool removeSolid(const Point3D& pos);
    bool contains(const Point3D& pos) const;
    static Point3D chunkOfPos(const Point3D& blockPos) { return qFloor(blockPos / Chunk::chunkSize); }
    static Point3D chunkOrigin(const Point3D& chunkPos) { return qFloor(chunkPos) * Chunk::chunkSize; }
    static Point3D relativePosOfPos(const Point3D& blockPos) { return blockPos - chunkOrigin(chunkOfPos(blockPos)); }

    HRect3D getMaxGeometry() const override { return maxGeometry; }
    Point3D getMiddleGeometry() const { return middleMinGeometry; }
    const QList<Solid*>* getSolids() const { return &solids; }
private:
    QList<Solid*> solids;
    //indépendants de la taille du chunk (presque) mais dépendant de la taille des blocks :
    HRect3D maxGeometry;
    Point3D middleMinGeometry;
    void calcMinMaxPoint();

    bool deleteSolid(Solid* block);
};



class WorldChange
{
public:
    enum Type {
        chunk,//dispo: chunk
        block,//dispo: chunk et block
        entity//dispo: entitée (et chunk?)
    };
    enum Action {
        removed,//pointeur vers l'objet mais pointe vers un objet delete
        created,
        moved,
        other
    };

    WorldChange(Type type, Action action);
    WorldChange(const WorldChange& change);
    void setChunk(Chunk* chunk) { tchunk = chunk; }
    void setSolid(Solid* block) { tblock = block; }
    void setEntity(Entity* entity) { tentity = entity; }
    Type type() const { return t; }
    Action action() const { return act; }
    Chunk* getChunk() const { return tchunk; }
    Solid* getSolid() const { return tblock; }
    Entity* getEntity() const { return tentity; }
private:
    Type t;
    Action act;
    Chunk* tchunk = nullptr;
    Solid* tblock = nullptr;
    Entity* tentity = nullptr;
};


class World : public QObject
{
    Q_OBJECT
public:
    World();
    ~World();
    bool addSolid(Solid* block);
    void addEntity(Entity* entity);

    QList<Chunk*> getChunks() const { return chunks; }
    QList<Entity*> getEntities() const { return entities; }
signals:
    void changed(const WorldChange& change);

private:
    QList<Chunk*> chunks;
    QList<Entity*> entities;
    bool createChunk(Point3D pos);
    void deleteChunk(Chunk* chunk);
    Chunk* getChunk(Point3D pos) const;
    bool haveChunk(Point3D pos) const;

    bool deleteEntity(Entity* entity);
};

#endif // WORLD3D_H
