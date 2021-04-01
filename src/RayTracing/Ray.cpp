#include "Ray.h"

Ray::Ray(const Pos3D &pos, RayTracingRessources *rtRess) : pos(pos), rtRess(rtRess)
{
    distParcouru = 0;
    insideMaterial = rtRess->insideMaterial;
    float newSpeed = OBJECT3D::getSpeedOfLight(insideMaterial);
    float previousSpeed = 1; // the speed of light in the lens
    if (newSpeed != previousSpeed) {
        // calcul de la réfraction (le regard est normal au plan)
        this->pos.setRot(Transfo3D::refractRot(rtRess->clientPos, pos, newSpeed / previousSpeed));
    }
#ifdef NAN_VERIF
    if (this->pos.isNan()) {
        qDebug() << "Ray::Ray pos is nan" << this->pos;
        exit(-1);
    }
#endif // NAN_VERIF
}

ColorLight Ray::getColor() const
{
    if (colors.isEmpty()) return ColorLight(0, 0, 0, 255, 0);
    ColorLight retour = colors.last();
    for (int i = colors.size() - 2; i >= 0; i--)
        retour += colors.at(i);
    return retour;
}

void Ray::process(const World *world)
{
    while (distParcouru < viewDistance) { // pas exactement view distance mais pas loins (2*..?)
        //qint64 start = rtRess->dt->getCurrent();
        Point3D pInter;
        const Face *face = getFirstIntersection(world, pInter);
        //rtRess->dt->addValue("Ray::process_face", rtRess->dt->getCurrent() - start);
        if (face == nullptr || !face->isValid()) break; // c'est normal si on va dans le vide

        lastMoved = pInter != pos;
        lastFace = face;

        const QPointF pTexture = face->getPlan()->projeteOrtho(pInter) - face->getPointC();
        ColorLight color = face->getColor(pTexture, getTexture(face->getTexture()));
        colors.append(color);

        opacity += color.alpha();
        if (opacity >= 255) break; // le rayon est saturé (solide opaque)

        moveTo(pInter);
        if (face->getMaterial() == BLOCK::Material::mirror) {
            pos.setRot(face->boundRot(pos.getRot()));
        } else {
            float newSpeed = OBJECT3D::getSpeedOfLight(face->getMaterial());
            if (newSpeed <= 0) break; // un solide
            float previousSpeed = OBJECT3D::getSpeedOfLight(insideMaterial);
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

const Face *Ray::getFirstIntersection(const World *world, Point3D &pInter) const
{
    const Face *faceMin = nullptr;
    Point3D posNextPoint = pos.getNextPoint();
#ifdef NAN_VERIF
    if (posNextPoint.isNan()) {
        qDebug() << "Ray::getFirstIntersection posNextPoint is nan" << posNextPoint;
        exit(-1);
    }
#endif // NAN_VERIF
    Point3D pInterMin;
    doubli distanceInterMin = 0; // seront set car !faceMin.isValid() au début
    doubli distanceSolidMin = 0;
    for (int iChunk = 0; iChunk < world->getChunks().size(); iChunk++) {
        const Chunk *chunk = world->getChunks().at(iChunk);
        if (chunk->getPoint().distance(pos) > viewDistance)
            continue;
        //qint64 start1 = rtRess->dt->getCurrent();
        for (int i2 = 0; i2 < chunk->getSolids()->size(); i2++) {
            Solid *block = chunk->getSolids()->at(i2);
            if (block->getPoint().distance(pos) > viewDistance)
                continue;
            for (int i3 = 0; i3 < block->getFaces()->size(); i3++) {
                const Face *face = &block->getFaces()->at(i3);
                if (face == lastFace)
                    continue;
                const Plan *plan = face->getPlan();

                if (!plan->isValid())
                    continue;

                Point3DCancelable pInter = plan->intersection(pos, posNextPoint);

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

const QImage *Ray::getTexture(const QString &file) const
{
    if (!rtRess->facesImg->contains(file)) {
        QImage img = OBJECT3D::getTexture(file);
        if (img.isNull()) {
            rtRess->facesImg->insert(file, nullptr);
        } else {
            rtRess->facesImg->insert(file, new QImage(img));
        }
    }
    return rtRess->facesImg->value(file);
}
