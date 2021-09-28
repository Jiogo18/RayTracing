#ifndef WORLD3D_H
#define WORLD3D_H

#include "../3D/Solid.h"
#include "../3D/Entity.h"
#include <vector>
#include <algorithm>

class ChunkPos : public Point3D
{
public:
    static const int chunkSize = 8;

    static ChunkPos fromBlockPos(const Point3D &blockPos) { return qRound(blockPos / ChunkPos::chunkSize); }
    Point3D chunkOrigin() const { return qRound(Point3D::operator*(ChunkPos::chunkSize)); }
    constexpr inline ChunkPos(const ChunkPos &pos) : Point3D(pos) {}

private:
    constexpr inline ChunkPos(const Point3D &pos) : Point3D(pos) {}
};

class Chunk : public ChunkPos
{
public:
    inline Chunk(ChunkPos pos) : ChunkPos(pos) { calcMinMaxPoint(); }
    ~Chunk();
    constexpr inline const ChunkPos &getPoint() const { return *this; }

    bool addSolid(Solid *block);
    bool haveSolid(const Point3D &blockPos) const;
    Solid *getSolidAt(const Point3D &blockPos) const;
    Solid *getSolid(const Point3D &blockPos) const;
    bool removeSolid(const Point3D &blockPos);
    bool containsPoint(const Point3D &blockPos) const;
    static Point3D relativePosOfPos(const Point3D &blockPos) { return blockPos - ChunkPos::fromBlockPos(blockPos).chunkOrigin(); }

    const HRect3D &getMaxGeometry() const { return maxGeometry; }
    const Point3D &getMiddleGeometry() const { return middleMinGeometry; }
    const std::vector<Solid *> *getSolids() const { return &solids; }
    static Size3D getSize() { return Size3D(ChunkPos::chunkSize, ChunkPos::chunkSize, ChunkPos::chunkSize); }

private:
    std::vector<Solid *> solids;
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

    constexpr inline WorldChange(Type type, Action action) : t(type), act(action) {}
    constexpr inline WorldChange(const WorldChange &c) : t(c.t), act(c.act), tchunk(c.tchunk), tblock(c.tblock), tentity(c.tentity) {}
    void setChunk(Chunk *chunk) { tchunk = chunk; }
    void setSolid(Solid *block) { tblock = block; }
    void setEntity(Entity *entity) { tentity = entity; }
    const Type &type() const { return t; }
    const Action &action() const { return act; }
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

class World
{
public:
    World() {}
    ~World();
    bool addSolid(Solid *block);
    void addEntity(Entity *entity);

    const std::vector<Chunk *> &getChunks() const { return chunks; }
    const std::vector<Entity *> &getEntities() const { return entities; }

    Solid *getSolid(const Point3D &point) const;

    bool removeSolid(const Point3D &solidPos);

    // signals:
    void changed(const WorldChange &change);

private:
    std::vector<Chunk *> chunks;
    std::vector<Entity *> entities;
    bool createChunk(const ChunkPos &posChunk);
    void deleteChunk(Chunk *chunk);
    Chunk *getChunk(const ChunkPos &posChunk) const;
    bool haveChunk(const ChunkPos &posChunk) const;

    bool deleteEntity(Entity *entity);
};

#endif // WORLD3D_H
