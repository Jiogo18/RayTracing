#ifndef ENTITY_H
#define ENTITY_H

#include "Object3D.h"

namespace ENTITY {
    enum Type {
        none,
        player,
        ghost //spectator
    };
    enum Direction { front,
                     back,
                     left,
                     right,
                     top,
                     bottom };
    doubli baseSpeed(ENTITY::Type type, ENTITY::Direction dir);
} // namespace ENTITY

class EntityAttribute
{
public:
    EntityAttribute() { type = ENTITY::Type::none; }
    EntityAttribute(ENTITY::Type type);
    EntityAttribute(const EntityAttribute &attribute);
    void setType(ENTITY::Type type) { this->type = type; }
    ENTITY::Type getType() const { return type; }
    doubli getSpeed(ENTITY::Direction dir) const;
    void setSpeed(ENTITY::Direction dir, doubli speed)
    {
        this->speed.remove(dir);
        this->speed.insert(dir, speed);
    }
    doubli getSpeedF() const { return getSpeed(ENTITY::Direction::front); }
    doubli getSpeedB() const { return getSpeed(ENTITY::Direction::back); }
    doubli getSpeedL() const { return getSpeed(ENTITY::Direction::left); }
    doubli getSpeedR() const { return getSpeed(ENTITY::Direction::right); }
    doubli getSpeedU() const { return getSpeed(ENTITY::Direction::top); }
    doubli getSpeedD() const { return getSpeed(ENTITY::Direction::bottom); }

private:
    QMap<int, doubli> speed;
    ENTITY::Type type;
};

class Entity : public Object
{
public:
    Entity(const Pos3D &pos, ENTITY::Type type);

    void moveUp() { addZ(attribute.getSpeedU()); }
    void moveDown() { addZ(-attribute.getSpeedD()); }
    void moveFront() { moveWithRot(attribute.getSpeedF(), 0); }
    void moveBack() { moveWithRot(attribute.getSpeedB(), M_PI); }
    void moveLeft() { moveWithRot(attribute.getSpeedL(), -M_PI / 2); }
    void moveRight() { moveWithRot(attribute.getSpeedR(), M_PI / 2); }
    void moveTo(const Point3D &point) { setPoint(point); }
    void moveTo(const Pos3D &pos)
    {
        moveTo(static_cast<Point3D>(pos));
        setRX(pos.rX());
        setRZ(pos.rZ());
    }
    void moveRX(radian rX) { addRX(rX); }
    void moveRZ(radian rZ) { addRZ(rZ); }

private:
    EntityAttribute attribute;
};

#endif // ENTITY_H
