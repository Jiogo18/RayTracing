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

EntityAttribute::EntityAttribute(ENTITY::Type type) : type(type)
{
    speed[ENTITY::Direction::front] = baseSpeed(type, ENTITY::Direction::front);
    speed[ENTITY::Direction::back] = baseSpeed(type, ENTITY::Direction::back);
    speed[ENTITY::Direction::left] = baseSpeed(type, ENTITY::Direction::left);
    speed[ENTITY::Direction::right] = baseSpeed(type, ENTITY::Direction::right);
    speed[ENTITY::Direction::top] = baseSpeed(type, ENTITY::Direction::top);
    speed[ENTITY::Direction::bottom] = baseSpeed(type, ENTITY::Direction::bottom);
}
EntityAttribute::EntityAttribute(const EntityAttribute &obj) : type(obj.type)
{
    speed[ENTITY::Direction::front] = obj.speed[ENTITY::Direction::front];
    speed[ENTITY::Direction::back] = obj.speed[ENTITY::Direction::back];
    speed[ENTITY::Direction::left] = obj.speed[ENTITY::Direction::left];
    speed[ENTITY::Direction::right] = obj.speed[ENTITY::Direction::right];
    speed[ENTITY::Direction::top] = obj.speed[ENTITY::Direction::top];
    speed[ENTITY::Direction::bottom] = obj.speed[ENTITY::Direction::bottom];
}

/*****************************************************************************
  Entity
 *****************************************************************************/

Entity::Entity(const Pos3D &pos, ENTITY::Type type) : Object(pos) { attribute.setType(type); }
