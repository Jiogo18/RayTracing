#ifndef RAY_H
#define RAY_H

#include "Ressources.h"

class Ray
{
public:
    constexpr inline Ray(const Pos3D &pos, RayTracingRessources *rtRess);
    ColorLight getColor(const int &baseLight) const;
    void process();

private:
    Pos3D pos;
    QList<ColorLight> colors;
    const Face *lastFace = nullptr;
    bool lastMoved = false;
    int opacity = 0;
    // vector objTraverse;
    doubli distParcouru;
    void moveTo(const Point3D &newPos);
    const Face *getFirstIntersection(Point3D &pInterMin) const;
    // black list (et white list pour transparence) du dernier objet traversé
    // par défaut: sortie du client (pour pas le retapper)
    // si rebond: dernier objet en mode "sortie"
    // si entrée (transparent): dernier objet en mode "entrée" puis "sortie" pour calculer l'épaisseur
    // si un objet dans l'objet entré est touché alors on sort du premier objet au point P et on rentre ds le 2
    // puis on resort du 2 on rerentre ds le 1 et resort du 1 (de l'autre coté de 2)
    bool enter = false;
    SOLID::Material insideMaterial;
    RayTracingRessources *rtRess = nullptr;
};

constexpr inline Ray::Ray(const Pos3D &pos, RayTracingRessources *rtRess) : pos(pos), distParcouru(0), insideMaterial(rtRess->insideMaterial), rtRess(rtRess)
{
    float newSpeed = SOLID::getSpeedOfLight(insideMaterial);
    //float previousSpeed = 1; // the speed of light in the lens
    if (newSpeed != 1) {
        // calcul de la réfraction (le regard est normal au plan)
        this->pos.setRot(Transfo3D::refractRot(rtRess->clientPos, pos, newSpeed));
    }
#ifdef NAN_VERIF
    if (this->pos.isNan()) {
        qDebug() << "Ray::Ray pos is nan" << this->pos;
        exit(-1);
    }
#endif // NAN_VERIF
}

#endif // RAY_H
