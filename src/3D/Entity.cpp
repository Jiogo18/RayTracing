#include "Entity.h"

/*****************************************************************************
  Static functions
 *****************************************************************************/

doubli ENTITY::baseSpeed(ENTITY::Type type, ENTITY::Direction dir)
{
    doubli speed = 0;
    switch (type) {
    case ENTITY::Type::none:
        speed = 0;
        break;
    case ENTITY::Type::player:
    case ENTITY::Type::ghost:
        speed = 0.1L;
        break;
    }
    switch (type) {
    case ENTITY::Type::player:
        switch (dir) {
        case ENTITY::Direction::left:
        case ENTITY::Direction::right:
        case ENTITY::Direction::back:
            speed /= 2;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    return roundNull(speed);
}

/*****************************************************************************
  EntityAttribute
 *****************************************************************************/

EntityAttribute::EntityAttribute(ENTITY::Type type) { this->type = type; }
EntityAttribute::EntityAttribute(const EntityAttribute &obj)
{
    type = obj.type;
    speed = obj.speed;
}
doubli EntityAttribute::getSpeed(ENTITY::Direction dir) const
{
    if (speed.contains(dir))
        return speed.value(dir);
    else
        return baseSpeed(type, dir);
}

/*****************************************************************************
  Entity
 *****************************************************************************/

Entity::Entity(const Pos3D &pos, ENTITY::Type type) : Object(pos) { attribute.setType(type); }
