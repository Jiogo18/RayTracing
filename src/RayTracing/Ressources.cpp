#include "Ressources.h"

RayTracingRessources::RayTracingRessources(const World *world, const Entity *client, DebugTime *dt)
{
    this->world = world;
    facesImg = new QMap<QString, const QImage *>;
    this->dt = dt;
    resetRessources(client);
}

RayTracingRessources::~RayTracingRessources()
{
    if (facesImg != nullptr) {
        QMapIterator<QString, const QImage *> i(*facesImg);
        while (i.hasNext()) {
            i.next();
            delete i.value();
        }
        delete facesImg;
    }
}

void RayTracingRessources::onWorldChanged() {}

void RayTracingRessources::resetRessources(const Entity *client)
{
    clientPos = client->getPos();
    const Solid *solid = world->getSolid(clientPos);
    insideMaterial = solid ? solid->getMaterial() : BLOCK::Material::air; // par défaut on est dans l'air
}
