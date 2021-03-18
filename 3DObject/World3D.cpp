#include "World3D.h"





Chunk::Chunk(Point3D pos) : Object(Pos3D(pos*chunkSize, 0, 0))
{
    calcMinMaxPoint();
}
Chunk::~Chunk()
{
    while(blocks.isEmpty())
        deleteBlock(blocks.first());
}

bool Chunk::setBlock(Block *block)
{
    if(!contains(block->getPoint()))
        return false;
    if(haveBlock(block->getPoint())) {
        removeBlock(block->getPoint());
    }
    blocks.append(block);
    calcMinMaxPoint();
    return true;
}
bool Chunk::haveBlock(const Point3D &pos) const
{ return getBlock(pos) != nullptr; }
Block *Chunk::getBlock(const Point3D &pos) const
{
    for(int i=0; i<blocks.size(); i++)
        if(blocks.at(i)->getPoint() == pos)
            return blocks.at(i);
    return nullptr;
}

bool Chunk::removeBlock(const Point3D &pos)
{
    Block *block = getBlock(pos);
    if(!block) return false;
    blocks.removeAll(block);
    delete block;
    return true;
}

bool Chunk::contains(const Point3D &pos) const { return chunkOfPos(pos) == getPoint(); }

void Chunk::calcMinMaxPoint()
{
    if(blocks.size() == 0) {
        maxGeometry = HRect3D(getPoint(), getPoint());
        return;
    }
    maxGeometry = blocks.first()->getMaxGeometry();
    doubli minX = maxGeometry.getPointMin().getX();
    doubli minY = maxGeometry.getPointMin().getY();
    doubli minZ = maxGeometry.getPointMin().getZ();
    doubli maxX = maxGeometry.getPointMax().getX();
    doubli maxY = maxGeometry.getPointMax().getY();
    doubli maxZ = maxGeometry.getPointMax().getZ();

    for(int i=0; i<blocks.size(); i++) {
        HRect3D currentMaxGeometry = blocks.at(i)->getMaxGeometry();
        if(currentMaxGeometry.getPointMin().getX() < minX)
            minX = currentMaxGeometry.getPointMin().getX();
        if(currentMaxGeometry.getPointMin().getY() < minY)
            minY = currentMaxGeometry.getPointMin().getY();
        if(currentMaxGeometry.getPointMin().getZ() < minZ)
            minZ = currentMaxGeometry.getPointMin().getZ();
        if(maxX < currentMaxGeometry.getPointMax().getX())
            maxX = currentMaxGeometry.getPointMax().getX();
        if(maxY < currentMaxGeometry.getPointMax().getY())
            maxY = currentMaxGeometry.getPointMax().getY();
        if(maxZ < currentMaxGeometry.getPointMax().getZ())
            maxZ = currentMaxGeometry.getPointMax().getZ();
    }
    maxGeometry = HRect3D(Point3D(minX, minY, minZ), Point3D(maxX, maxY, maxZ));
    //middleMinGeometry = Point3D((minX+maxX)/2, (minY+maxY)/2, minZ);
    middleMinGeometry = maxGeometry.getMiddle();
}

bool Chunk::deleteBlock(Block *block)
{
    if(!blocks.contains(block))
        return false;
    blocks.removeAll(block);
    if(block != nullptr)
        delete block;
    return true;
}




WorldChange::WorldChange(WorldChange::Type type, Action action) { t = type; act = action; }
WorldChange::WorldChange(const WorldChange &change)
{
    t=change.t;
    tchunk=change.tchunk;
    tblock=change.tblock;
    tentity=change.tentity;
}


World::World() {}
World::~World()
{
    while(!chunks.isEmpty())
        deleteChunk(chunks.first());
    while(!entities.isEmpty())
        deleteEntity(entities.first());
}

bool World::setBlock(Block *block)
{
    Point3D posChunk = Chunk::chunkOfPos(block->getPoint());
    if(!haveChunk(posChunk))
        if(!createChunk(posChunk)) {
            qDebug() << "ERROR Chunk::setBlock can't create a chunk";
            return false;
        }
    bool success = getChunk(posChunk)->setBlock(block);
    if(success) {
        WorldChange change(WorldChange::Type::block, WorldChange::Action::created);
        change.setBlock(block);
        change.setChunk(getChunk(posChunk));
        emit changed(change);
    }
    return success;
}
void World::addEntity(Entity *entity)
{
    if(entities.contains(entity))
        return;
    entities.append(entity);
    WorldChange change(WorldChange::Type::entity, WorldChange::Action::created);
    change.setEntity(entity);
    emit changed(change);
}

bool World::createChunk(Point3D pos)
{
    if(haveChunk(Chunk::chunkOfPos(pos)))
        return false;
    chunks.append(new Chunk(pos));
    WorldChange change(WorldChange::Type::chunk, WorldChange::Action::created);
    change.setChunk(getChunk(pos));
    emit changed(change);

    return haveChunk(Chunk::chunkOfPos(pos));
}
void World::deleteChunk(Chunk *chunk)
{
    chunks.removeAll(chunk);
    if(chunk != nullptr)
        delete chunk;
    WorldChange change(WorldChange::Type::chunk, WorldChange::Action::removed);
    change.setChunk(chunk);
    emit changed(change);
}
Chunk *World::getChunk(Point3D pos) const
{
    for(int i=0; i<chunks.size(); i++)
        if(chunks.at(i)->getPoint() == pos)
            return chunks.at(i);
    return nullptr;
}
bool World::haveChunk(Point3D pos) const
{
    return getChunk(pos) != nullptr;
}

bool World::deleteEntity(Entity *entity)
{
    if(!entities.contains(entity))
        return false;
    entities.removeAll(entity);
    if(entity != nullptr)
        delete entity;
    WorldChange change(WorldChange::Type::entity, WorldChange::Action::removed);
    change.setEntity(entity);
    emit changed(change);
    return true;
}
