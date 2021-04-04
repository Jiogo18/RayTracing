#include "Ressources.h"

RayTracingRessources::RayTracingRessources(const World *world, const Entity *client, DebugTime *dt) : world(world), dt(dt)
{
    resetRessources(client);
}

void RayTracingRessources::onWorldChanged() {}

void RayTracingRessources::resetRessources(const Entity *client)
{
    clientPos = client->getPos();
    const Solid *solid = world->getSolid(clientPos);
    insideMaterial = solid ? solid->getMaterial() : SOLID::Material::air; // par défaut on est dans l'air
}
