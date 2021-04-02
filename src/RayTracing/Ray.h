#ifndef RAY_H
#define RAY_H

#include "Ressources.h"

class Ray
{
public:
    Ray(const Pos3D &pos, RayTracingRessources *rtRess);
    ColorLight getColor(const int &baseLight) const;
    void process(const World *world);

private:
    Pos3D pos;
    QList<ColorLight> colors;
    const Face *lastFace = nullptr;
    bool lastMoved = false;
    int opacity = 0;
    // vector objTraverse;
    doubli distParcouru;
    void moveTo(const Point3D &newPos);
    const Face *getFirstIntersection(const World *world, Point3D &pInterMin) const;
    // black list (et white list pour transparence) du dernier objet traversé
    // par défaut: sortie du client (pour pas le retapper)
    // si rebond: dernier objet en mode "sortie"
    // si entrée (transparent): dernier objet en mode "entrée" puis "sortie" pour calculer l'épaisseur
    // si un objet dans l'objet entré est touché alors on sort du premier objet au point P et on rentre ds le 2
    // puis on resort du 2 on rerentre ds le 1 et resort du 1 (de l'autre coté de 2)
    bool enter = false;
    BLOCK::Material insideMaterial;
    RayTracingRessources *rtRess = nullptr;
    const QImage *getTexture(const QString &file) const;
};

#endif // RAY_H
