#include "Ray.h"

ColorLight Ray::getColor(const int &baseLight) const
{
    ColorLight retour{0, 0, 0, 0, baseLight};
    if (colors.isEmpty()) return retour;
    for (int i = colors.size() - 1; i >= 0; i--)
        retour += colors.at(i);
    return retour;
}

void Ray::process()
{
    while (distParcouru < viewDistance) { // pas exactement view distance mais pas loins (2*..?)
        //qint64 start = rtRess->dt->getCurrent();
        Point3D pInter;
        const Face *face = getFirstIntersection(pInter);
        //rtRess->dt->addValue("Ray::process_face", rtRess->dt->getCurrent() - start);
        if (face == nullptr || !face->isValid()) break; // c'est normal si on va dans le vide

        lastMoved = pInter != pos;
        lastFace = face;

        const QPointF pTexture = face->getPlan()->projeteOrtho(pInter) - face->getPointC();
        const ColorLight color = face->getColor(pTexture);
        colors.append(color);

        opacity += color.alpha();
        if (opacity >= 255) break; // le rayon est saturé (solide opaque)

        moveTo(pInter);
        if (face->getMaterial() == SOLID::Material::mirror) {
            pos.setRot(face->boundRot(pos.getRot()));
        } else {
            float newSpeed = SOLID::getSpeedOfLight(face->getMaterial());
            if (newSpeed <= 0) break; // un solide
            float previousSpeed = SOLID::getSpeedOfLight(insideMaterial);
            // calcul de la réfraction
            pos.setRot(face->refractRot(pos, newSpeed / previousSpeed));
            //moveTo(Pos3D(pInter, pos.getRX(), pos.getRZ()));
            insideMaterial = face->getMaterial();
            // on ne "rentre" pas dans le miroir (ou sinon c'est juste la couche de verre)
        }

        //break;// TODO empecher de calculer après pour les tests (avoir que la première face)
        // Tips: transparant il faut penser à passer sur la face de l'autre coté... trop complexe :'(

#ifdef NAN_VERIF
        if (pos.isNan()) {
            //étrangement dans le constructeur il est pas nan ?
            qDebug() << "Ray::process pos is nan" << pos;
            exit(-1);
        }
#endif // NAN_VERIF
    }
}

void Ray::moveTo(const Point3D &newPos)
{
    distParcouru += pos.distance(newPos);
    pos.setPoint(newPos);
}

const Face *Ray::getFirstIntersection(Point3D &pInter) const
{
    const Face *faceMin = nullptr;
    Point3D posNextPoint = pos.getNextPoint();
    Vec3D vecDirNextPoint(posNextPoint - static_cast<Point3D>(pos));

#ifdef NAN_VERIF
    if (posNextPoint.isNan()) {
        qDebug() << "Ray::getFirstIntersection posNextPoint is nan" << posNextPoint;
        exit(-1);
    }
#endif // NAN_VERIF
    Point3D pInterMin;
    doubli distanceInterMin = 0; // seront set car !faceMin.isValid() au début
    doubli distanceSolidMin = 0;
    for (int iChunk = 0; iChunk < rtRess->world->getChunks().size(); iChunk++) {
        const Chunk *chunk = rtRess->world->getChunks().at(iChunk);
        if (chunk->getPoint().distance(pos) > viewDistance)
            continue;
        if (!chunk->getMaxGeometry().containsLine(pos, vecDirNextPoint))
            continue;
        //qint64 start1 = rtRess->dt->getCurrent();
        for (int i2 = 0; i2 < chunk->getSolids()->size(); i2++) {
            const Solid *block = chunk->getSolids()->at(i2);
            if (block->getPoint().distance(pos) > viewDistance)
                continue;
            if (!block->getMaxGeometry().containsLine(pos, vecDirNextPoint))
                continue;
            for (int i3 = 0; i3 < block->getFaces()->size(); i3++) {
                const Face *face = &block->getFaces()->at(i3);
                if (face == lastFace)
                    continue;
                const Plan *plan = face->getPlan();

                if (!plan->isValid())
                    continue;

                const Point3DCancelable pInter = plan->intersection(pos, posNextPoint);

                if (!pInter.isValid() || !face->getMaxGeometry().contains(pInter))
                    continue;
                if (!enter && pInter == pos && !lastMoved)
                    continue; // on viens de sortir (ou par défaut) et on se retrouve sur le même point
                doubli distanceInter = pos.distance(pInter), distanceSolid = 0;
                if (faceMin != nullptr && faceMin->isValid()) {
                    if (distanceInter > distanceInterMin)
                        continue;
                    distanceSolid = pos.distance(block->getMiddleGeometry());
                    if (distanceInter == distanceInterMin) {
                        if (distanceSolid >= distanceSolidMin)
                            continue;
                    }
                } else {
                    distanceSolid = pos.distance(block->getMiddleGeometry());
                }

                // si il n'y a pas encore de face
                // sinon si le pt d'intersection est plus proche
                // sinon si c'est le même et si la face est plus proche
                // sinon si c'est la même et si le block est plus proche
                // alors on prend
                faceMin = face;
                pInterMin = pInter;
                distanceInterMin = distanceInter;
                distanceSolidMin = distanceSolid;
                // il est possible qu'un même distance soit pour le meme block, pour des faces diff (de meme distance)
                // mais c'est pas grave car ça arrive quand c'est VRAIMENT indéterminant
                //(on peu prendre l'un ou l'autre)
            }
        }
        //rtRess->dt->addValue("Ray::firstIntersection_1", rtRess->dt->getCurrent() - start1);
    }
    pInter = pInterMin;
    return faceMin;
}
