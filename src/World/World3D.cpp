#include "World3D.h"

Chunk::~Chunk()
{
    while (solids.isEmpty())
        deleteSolid(solids.first());
}

bool Chunk::addSolid(Solid *block)
{
    if (!containsPoint(block->getPoint())) return false;
    if (haveSolid(block->getPoint())) {
        removeSolid(block->getPoint());
    }
    solids.append(block);
    calcMinMaxPoint();
    return true;
}
bool Chunk::haveSolid(const Point3D &blockPos) const { return getSolidAt(blockPos) != nullptr; }
Solid *Chunk::getSolidAt(const Point3D &blockPos) const
{
    for (int i = 0; i < solids.size(); i++)
        if (solids.at(i)->getPoint() == blockPos)
            return solids.at(i);
    return nullptr;
}

Solid *Chunk::getSolid(const Point3D &blockPos) const
{
    for (int i = 0; i < solids.size(); i++)
        if (solids.at(i)->containsPoint(blockPos))
            return solids.at(i);
    return nullptr;
}

bool Chunk::removeSolid(const Point3D &blockPos)
{
    Solid *block = getSolidAt(blockPos);
    if (!block) return false;
    solids.removeAll(block);
    delete block;
    return true;
}

bool Chunk::containsPoint(const Point3D &blockPos) const { return ChunkPos::fromBlockPos(blockPos) == getPoint(); }

void Chunk::calcMinMaxPoint()
{
    if (solids.size() == 0) {
        maxGeometry = HRect3D(getPoint(), getPoint());
        return;
    }
    maxGeometry = solids.first()->getMaxGeometry();
    doubli minX = maxGeometry.getPointMin().x();
    doubli minY = maxGeometry.getPointMin().y();
    doubli minZ = maxGeometry.getPointMin().z();
    doubli maxX = maxGeometry.getPointMax().x();
    doubli maxY = maxGeometry.getPointMax().y();
    doubli maxZ = maxGeometry.getPointMax().z();

    for (int i = 0; i < solids.size(); i++) {
        HRect3D currentMaxGeometry = solids.at(i)->getMaxGeometry();
        if (currentMaxGeometry.getPointMin().x() < minX)
            minX = currentMaxGeometry.getPointMin().x();
        if (currentMaxGeometry.getPointMin().y() < minY)
            minY = currentMaxGeometry.getPointMin().y();
        if (currentMaxGeometry.getPointMin().z() < minZ)
            minZ = currentMaxGeometry.getPointMin().z();
        if (maxX < currentMaxGeometry.getPointMax().x())
            maxX = currentMaxGeometry.getPointMax().x();
        if (maxY < currentMaxGeometry.getPointMax().y())
            maxY = currentMaxGeometry.getPointMax().y();
        if (maxZ < currentMaxGeometry.getPointMax().z())
            maxZ = currentMaxGeometry.getPointMax().z();
    }
    maxGeometry = HRect3D(Point3D(minX, minY, minZ), Point3D(maxX, maxY, maxZ));
    //middleMinGeometry = Point3D((minX+maxX)/2, (minY+maxY)/2, minZ);
    middleMinGeometry = maxGeometry.getMiddle();
}

bool Chunk::deleteSolid(Solid *block)
{
    if (!solids.contains(block)) return false;
    solids.removeAll(block);
    if (block != nullptr) delete block;
    return true;
}

World::~World()
{
    while (!chunks.isEmpty())
        deleteChunk(chunks.first());
    while (!entities.isEmpty())
        deleteEntity(entities.first());
}

bool World::addSolid(Solid *block)
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
void World::addEntity(Entity *entity)
{
    if (entities.contains(entity)) return;
    entities.append(entity);
    WorldChange change(WorldChange::Type::entity, WorldChange::Action::created);
    change.setEntity(entity);
    emit changed(change);
}

Solid *World::getSolid(const Point3D &point) const
{
    const Chunk *chunk = getChunk(ChunkPos::fromBlockPos(point));
    return chunk ? chunk->getSolid(point) : nullptr;
}

bool World::createChunk(const ChunkPos &posChunk)
{
    if (haveChunk(posChunk)) return false;
    chunks.append(new Chunk(posChunk));
    WorldChange change(WorldChange::Type::chunk, WorldChange::Action::created);
    change.setChunk(getChunk(posChunk));
    emit changed(change);

    return haveChunk(posChunk);
}
void World::deleteChunk(Chunk *chunk)
{
    chunks.removeAll(chunk);
    if (chunk != nullptr) delete chunk;
    WorldChange change(WorldChange::Type::chunk, WorldChange::Action::removed);
    change.setChunk(chunk);
    emit changed(change);
}
Chunk *World::getChunk(const ChunkPos &posChunk) const
{
    for (int i = 0; i < chunks.size(); i++)
        if (chunks.at(i)->getPoint() == posChunk)
            return chunks.at(i);
    return nullptr;
}
bool World::haveChunk(const ChunkPos &posChunk) const { return getChunk(posChunk) != nullptr; }

bool World::deleteEntity(Entity *entity)
{
    if (!entities.contains(entity)) return false;
    entities.removeAll(entity);
    if (entity != nullptr) delete entity;
    WorldChange change(WorldChange::Type::entity, WorldChange::Action::removed);
    change.setEntity(entity);
    emit changed(change);
    return true;
}
