#ifndef WORLD3D_H
#define WORLD3D_H

#include "Object3D.h"

class ChunkPos : public Point3D
{
public:
    static const int chunkSize = 8;

    static ChunkPos fromBlockPos(const Point3D &blockPos) { return qFloor(blockPos / ChunkPos::chunkSize); }
    Point3D chunkOrigin() const { return qFloor(static_cast<Point3D>(*this) * ChunkPos::chunkSize); }
    ChunkPos(const ChunkPos &pos) : Point3D(pos) {}

private:
    ChunkPos(const Point3D &pos) : Point3D(pos) {}
};

class Chunk : public ChunkPos
{
public:
    Chunk(ChunkPos pos);
    ~Chunk();
    ChunkPos getPoint() const { return *this; }

    bool addSolid(Solid *block);
    bool haveSolid(const Point3D &blockPos) const;
    Solid *getSolidAt(const Point3D &blockPos) const;
    Solid *getSolid(const Point3D &blockPos) const;
    bool removeSolid(const Point3D &blockPos);
    bool containsPoint(const Point3D &blockPos) const;
    static Point3D relativePosOfPos(const Point3D &blockPos) { return blockPos - ChunkPos::fromBlockPos(blockPos).chunkOrigin(); }

    HRect3D getMaxGeometry() const { return maxGeometry; }
    Point3D getMiddleGeometry() const { return middleMinGeometry; }
    const QList<Solid *> *getSolids() const { return &solids; }
    static Size3D getSize() { return Size3D(ChunkPos::chunkSize, ChunkPos::chunkSize, ChunkPos::chunkSize); }

private:
    QList<Solid *> solids;
    // indépendants de la taille du chunk (presque) mais dépendant de la taille des blocks :
    HRect3D maxGeometry;
    Point3D middleMinGeometry;
    void calcMinMaxPoint();

    bool deleteSolid(Solid *block);
};

class WorldChange
{
public:
    enum Type {
        chunk, // dispo: chunk
        block, // dispo: chunk et block
        entity // dispo: entitée (et chunk?)
    };
    enum Action {
        removed, // pointeur vers l'objet mais pointe vers un objet delete
        created,
        moved,
        other
    };

    WorldChange(Type type, Action action);
    WorldChange(const WorldChange &change);
    void setChunk(Chunk *chunk) { tchunk = chunk; }
    void setSolid(Solid *block) { tblock = block; }
    void setEntity(Entity *entity) { tentity = entity; }
    Type type() const { return t; }
    Action action() const { return act; }
    Chunk *getChunk() const { return tchunk; }
    Solid *getSolid() const { return tblock; }
    Entity *getEntity() const { return tentity; }

private:
    Type t;
    Action act;
    Chunk *tchunk = nullptr;
    Solid *tblock = nullptr;
    Entity *tentity = nullptr;
};

class World : public QObject
{
    Q_OBJECT
public:
    World();
    ~World();
    bool addSolid(Solid *block);
    void addEntity(Entity *entity);

    QList<Chunk *> getChunks() const { return chunks; }
    QList<Entity *> getEntities() const { return entities; }

    Solid *getSolid(const Point3D &point) const;

signals:
    void changed(const WorldChange &change);

private:
    QList<Chunk *> chunks;
    QList<Entity *> entities;
    bool createChunk(ChunkPos posChunk);
    void deleteChunk(Chunk *chunk);
    Chunk *getChunk(ChunkPos posChunk) const;
    bool haveChunk(ChunkPos posChunk) const;

    bool deleteEntity(Entity *entity);
};

#endif // WORLD3D_H
