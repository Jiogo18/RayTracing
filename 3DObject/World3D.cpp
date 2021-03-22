#include "World3D.h"





Chunk::Chunk(ChunkPos pos) : ChunkPos(pos)
{
    calcMinMaxPoint();
}
Chunk::~Chunk()
{
    while (solids.isEmpty())
        deleteSolid(solids.first());
}

bool Chunk::addSolid(Solid* block)
{
    if (!containsPoint(block->getPoint()))
        return false;
    if (haveSolid(block->getPoint())) {
        removeSolid(block->getPoint());
    }
    solids.append(block);
    calcMinMaxPoint();
    return true;
}
bool Chunk::haveSolid(const Point3D& blockPos) const
{
    return getSolidAt(blockPos) != nullptr;
}
Solid* Chunk::getSolidAt(const Point3D& blockPos) const
{
    for (int i = 0; i < solids.size(); i++)
        if (solids.at(i)->getPoint() == blockPos)
            return solids.at(i);
    return nullptr;
}

Solid* Chunk::getSolid(const Point3D& blockPos) const
{
    for (int i = 0; i < solids.size(); i++)
        if (solids.at(i)->containsPoint(blockPos))
            return solids.at(i);
    return nullptr;
}

bool Chunk::removeSolid(const Point3D& blockPos)
{
    Solid* block = getSolidAt(blockPos);
    if (!block) return false;
    solids.removeAll(block);
    delete block;
    return true;
}

bool Chunk::containsPoint(const Point3D& blockPos) const { return ChunkPos::fromBlockPos(blockPos) == getPoint(); }

void Chunk::calcMinMaxPoint()
{
    if (solids.size() == 0) {
        maxGeometry = HRect3D(getPoint(), getPoint());
        return;
    }
    maxGeometry = solids.first()->getMaxGeometry();
    doubli minX = maxGeometry.getPointMin().getX();
    doubli minY = maxGeometry.getPointMin().getY();
    doubli minZ = maxGeometry.getPointMin().getZ();
    doubli maxX = maxGeometry.getPointMax().getX();
    doubli maxY = maxGeometry.getPointMax().getY();
    doubli maxZ = maxGeometry.getPointMax().getZ();

    for (int i = 0; i < solids.size(); i++) {
        HRect3D currentMaxGeometry = solids.at(i)->getMaxGeometry();
        if (currentMaxGeometry.getPointMin().getX() < minX)
            minX = currentMaxGeometry.getPointMin().getX();
        if (currentMaxGeometry.getPointMin().getY() < minY)
            minY = currentMaxGeometry.getPointMin().getY();
        if (currentMaxGeometry.getPointMin().getZ() < minZ)
            minZ = currentMaxGeometry.getPointMin().getZ();
        if (maxX < currentMaxGeometry.getPointMax().getX())
            maxX = currentMaxGeometry.getPointMax().getX();
        if (maxY < currentMaxGeometry.getPointMax().getY())
            maxY = currentMaxGeometry.getPointMax().getY();
        if (maxZ < currentMaxGeometry.getPointMax().getZ())
            maxZ = currentMaxGeometry.getPointMax().getZ();
    }
    maxGeometry = HRect3D(Point3D(minX, minY, minZ), Point3D(maxX, maxY, maxZ));
    //middleMinGeometry = Point3D((minX+maxX)/2, (minY+maxY)/2, minZ);
    middleMinGeometry = maxGeometry.getMiddle();
}

bool Chunk::deleteSolid(Solid* block)
{
    if (!solids.contains(block))
        return false;
    solids.removeAll(block);
    if (block != nullptr)
        delete block;
    return true;
}




WorldChange::WorldChange(WorldChange::Type type, Action action) { t = type; act = action; }
WorldChange::WorldChange(const WorldChange& change)
{
    t = change.t;
    tchunk = change.tchunk;
    tblock = change.tblock;
    tentity = change.tentity;
}


World::World() {}
World::~World()
{
    while (!chunks.isEmpty())
        deleteChunk(chunks.first());
    while (!entities.isEmpty())
        deleteEntity(entities.first());
}

bool World::addSolid(Solid* block)
{
    ChunkPos posChunk = ChunkPos::fromBlockPos(block->getPoint());
    if (!haveChunk(posChunk))
        if (!createChunk(posChunk)) {
            qDebug() << "ERROR Chunk::setSolid can't create a chunk";
            return false;
        }
    bool success = getChunk(posChunk)->addSolid(block);
    if (success) {
        WorldChange change(WorldChange::Type::block, WorldChange::Action::created);
        change.setSolid(block);
        change.setChunk(getChunk(posChunk));
        emit changed(change);
    }
    return success;
}
void World::addEntity(Entity* entity)
{
    if (entities.contains(entity))
        return;
    entities.append(entity);
    WorldChange change(WorldChange::Type::entity, WorldChange::Action::created);
    change.setEntity(entity);
    emit changed(change);
}

Solid* World::getSolid(const Point3D& point) const
{
    const Chunk* chunk = getChunk(ChunkPos::fromBlockPos(point));
    return chunk ? chunk->getSolid(point) : nullptr;
}


bool World::createChunk(ChunkPos posChunk)
{
    if (haveChunk(posChunk))
        return false;
    chunks.append(new Chunk(posChunk));
    WorldChange change(WorldChange::Type::chunk, WorldChange::Action::created);
    change.setChunk(getChunk(posChunk));
    emit changed(change);

    return haveChunk(posChunk);
}
void World::deleteChunk(Chunk* chunk)
{
    chunks.removeAll(chunk);
    if (chunk != nullptr)
        delete chunk;
    WorldChange change(WorldChange::Type::chunk, WorldChange::Action::removed);
    change.setChunk(chunk);
    emit changed(change);
}
Chunk* World::getChunk(ChunkPos posChunk) const
{
    for (int i = 0; i < chunks.size(); i++)
        if (chunks.at(i)->getPoint() == posChunk)
            return chunks.at(i);
    return nullptr;
}
bool World::haveChunk(ChunkPos posChunk) const
{
    return getChunk(posChunk) != nullptr;
}

bool World::deleteEntity(Entity* entity)
{
    if (!entities.contains(entity))
        return false;
    entities.removeAll(entity);
    if (entity != nullptr)
        delete entity;
    WorldChange change(WorldChange::Type::entity, WorldChange::Action::removed);
    change.setEntity(entity);
    emit changed(change);
    return true;
}
