#include "RayTracing.h"



template<typename T>
PixScreen<T>::PixScreen(const QSize& size)
{
    screen = QList<QList<T>>(size.width());
    for (int i = 0; i < size.width(); i++) {
        screen[i] = QList<T>(size.height());
    }
}


RayTracingRessources::RayTracingRessources(const World* world, const Entity* client, DebugTime* dt)
{
    this->world = world;
    facesImg = new QMap<QString, const QImage*>;
    this->dt = dt;
    resetRessources(client);
}

RayTracingRessources::~RayTracingRessources()
{
    if (facesImg != nullptr) {
        QMapIterator<QString, const QImage*> i(*facesImg);
        while (i.hasNext()) {
            i.next();
            delete i.value();
        }
        delete facesImg;
    }
}


void RayTracingRessources::worldChanged()
{
}

void RayTracingRessources::resetRessources(const Entity* client)
{
    clientPos = client->getPos();
    const Solid* solid = world->getSolid(clientPos);
    insideMaterial = solid ? solid->getMaterial() : BLOCK::Material::air;// par défaut on est dans l'air
}




Ray::Ray(Pos3D pos, RayTracingRessources* rtRess)
{
    this->rtRess = rtRess;

    distParcouru = 0;
    this->pos = pos;
    insideMaterial = rtRess->insideMaterial;
    float newSpeed = OBJECT3D::getSpeedOfLight(insideMaterial);
    float previousSpeed = 1;//OBJECT3D::getSpeedOfLight(BLOCK::Material::air);// the speed of light in the lens
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
    if (colors.isEmpty())
        return ColorLight(0, 0, 0, 255, 0);
    ColorLight retour = colors.last();
    for (int i = colors.size() - 2; i >= 0; i--)
        retour += colors.at(i);
    return retour;
}

void Ray::process(const World* world)
{
    while (distParcouru < viewDistance) {//pas exactement view distance mais pas loins (2*..?)
        qint64 start = rtRess->dt->getCurrent();
        Point3D pInter;
        const Face* face = getFirstIntersection(world, pInter);
        rtRess->dt->addValue("Ray::process_face", rtRess->dt->getCurrent() - start);
        if (face == nullptr || !face->isValid())
            break;//c'est normal si on va dans le vide

        lastMoved = pInter != pos;
        lastFace = face;

        const QPointF pTexture = face->getPlan()->projeteOrtho(pInter) - face->getPointC();
        ColorLight color = face->getColor(pTexture, getTexture(face->getTexture()));
        colors.append(color);

        opacity += color.alpha();
        if (opacity >= 255)
            break;// le rayon est saturé (solide opaque)

        moveTo(pInter);
        if (face->getMaterial() == BLOCK::Material::mirror) {
            pos.setRot(face->boundRot(pos.getRot()));
        }
        else {
            float newSpeed = OBJECT3D::getSpeedOfLight(face->getMaterial());
            if (newSpeed <= 0) break;// un solide
            float previousSpeed = OBJECT3D::getSpeedOfLight(insideMaterial);
            // calcul de la réfraction
            pos.setRot(face->refractRot(pos, newSpeed / previousSpeed));
            //moveTo(Pos3D(pInter, pos.getRX(), pos.getRZ()));
            insideMaterial = face->getMaterial();
            // on ne "rentre" pas dans le miroir (ou sinon c'est juste la couche de verre)
        }

        //break;//TODO empecher de calculer après pour les tests (avoir que la première face)
        //Tips: transparant il faut penser à passer sur la face de l'autre coté... trop complexe :'(

#ifdef NAN_VERIF
        if (pos.isNan()) {
            //étrangement dans le constructeur il est pas nan ?
            qDebug() << "Ray::process pos is nan" << pos;
            exit(-1);
        }
#endif // NAN_VERIF
    }
}

void Ray::moveTo(const Point3D& newPos)
{
    distParcouru += pos.distance(newPos);
    pos.setPoint(newPos);
}

const Face* Ray::getFirstIntersection(const World* world, Point3D& pInter) const
{
    const Face* faceMin = nullptr;
    Point3D posNextPoint = pos.getNextPoint();
#ifdef NAN_VERIF
    if (posNextPoint.isNan()) {
        qDebug() << "Ray::getFirstIntersection posNextPoint is nan" << posNextPoint;
        exit(-1);
    }
#endif // NAN_VERIF
    Point3D pInterMin;
    doubli distanceInterMin = 0;//seront set car !faceMin.isValid() au début
    doubli distanceSolidMin = 0;
    for (int i = 0; i < world->getChunks().size(); i++) {
        Chunk* chunk = world->getChunks().at(i);
        if (chunk->getPoint().distance(pos) > viewDistance)
            continue;
        qint64 start1 = rtRess->dt->getCurrent();
        for (int i2 = 0; i2 < chunk->getSolids()->size(); i2++) {
            Solid* block = chunk->getSolids()->at(i2);
            if (block->getPoint().distance(pos) > viewDistance)
                continue;
            for (int i3 = 0; i3 < block->getFaces()->size(); i3++) {
                const Face* face = &block->getFaces()->at(i3);
                if (face == lastFace)
                    continue;
                const Plan* plan = face->getPlan();

                if (!plan->isValid()) continue;

                Point3D pInter = plan->intersection(pos, posNextPoint);

                if (!pInter.isValid() || !face->getMaxGeometry().contains(pInter))
                    continue;
                if (!enter && pInter == pos && !lastMoved)
                    continue;//on viens de sortir (ou par défaut) et on se retrouve sur le même point
                doubli distanceInter = pos.distance(pInter), distanceSolid = 0;
                if (faceMin != nullptr && faceMin->isValid()) {
                    if (distanceInter > distanceInterMin)
                        continue;
                    distanceSolid = pos.distance(block->getMiddleGeometry());
                    if (distanceInter == distanceInterMin) {
                        if (distanceSolid >= distanceSolidMin)
                            continue;
                    }
                }
                else {
                    distanceSolid = pos.distance(block->getMiddleGeometry());
                }

                //si il n'y a pas encore de face
                //sinon si le pt d'intersection est plus proche
                //sinon si c'est le même et si la face est plus proche
                //sinon si c'est la même et si le block est plus proche
                //alors on prend
                faceMin = face;
                pInterMin = pInter;
                distanceInterMin = distanceInter;
                distanceSolidMin = distanceSolid;
                //il est possible qu'un même distance soit pour le meme block, pour des faces diff (de meme distance)
                //mais c'est pas grave car ça arrive quand c'est VRAIMENT indéterminant
                //(on peu prendre l'un ou l'autre)
            }

        }
        rtRess->dt->addValue("Ray::firstIntersection_1", rtRess->dt->getCurrent() - start1);

    }
    pInter = pInterMin;
    return faceMin;
}

const QImage* Ray::getTexture(const QString& file) const
{
    if (!rtRess->facesImg->contains(file)) {
        QImage img = OBJECT3D::getTexture(file);
        if (img.isNull()) {
            rtRess->facesImg->insert(file, nullptr);
        }
        else {
            rtRess->facesImg->insert(file, new QImage(img));
        }
    }
    return rtRess->facesImg->value(file);
}






RayTracingWorker::RayTracingWorker(int workerId, RayTracingRessources* rtRess, QObject* parent) : QThread(parent)
{
    this->workerId = workerId;
    this->rtRess = rtRess;
}

RayTracingWorker* RayTracingWorker::setPrimaryWork(const QList<doubli>& yPosList, const QSize& sceneSize)
{
    this->yPosList = yPosList;
    this->sceneSize = sceneSize;
    return this;
}



QList<doubli> RayTracingWorker::calcyPosList(const int& sceneHeight)
{
    QList<doubli> yPosList(sceneHeight);
    for (int y = 0; y < sceneHeight; y++) {
        yPosList.replace(y, roundNull(static_cast<doubli>(1 - 2.0L * y / sceneHeight) * yMax));
    }
    return yPosList;
}


QList<Pos3D> RayTracingWorker::calcRaysPosColumn(const int& x, const QSize& sceneSize, const Pos3D& clientPos, const QList<doubli>& yPosList)
{
    QList<Pos3D> raysPos(sceneSize.height());
    //pos en % de pixmap.width/2 * xMax
    doubli xPos = roundNull(static_cast<doubli>(2.0L * x / sceneSize.width() - 1) * xMax);
    radian angleH = atan(xPos);
    for (int y = 0; y < sceneSize.height(); y++) {
        //pos en % de pixmap.height/2 * yMax
        doubli yPos = yPosList.at(y);
        doubli d = sqrt(sqr(xPos) + sqr(yPos) + 1);
        radian angleV = asin(yPos / d);

        raysPos.replace(y, clientPos.getChildRot(angleH, angleV));
    }

    return raysPos;
}

void RayTracingWorker::run() {
    int totalLight = 0;

    qint64 start = rtRess->dt->getCurrent();
    QList<Pos3D> initialPos = calcRaysPosColumn(xScene, sceneSize, rtRess->clientPos, yPosList);
    rtRess->dt->addValue("RayTracingWorker::run_calcRaysPosColumn", rtRess->dt->getCurrent() - start);

    start = rtRess->dt->getCurrent();
    QList<ColorLight> colors(initialPos.length());
    for (int i = 0; i < initialPos.length(); i++) {
        qint64 start2 = rtRess->dt->getCurrent();
        Ray ray(initialPos.at(i), rtRess);
        rtRess->dt->addValue("RayTracingWorker::run_1", rtRess->dt->getCurrent() - start2);

        start2 = rtRess->dt->getCurrent();
        ray.process(rtRess->world);
        rtRess->dt->addValue("RayTracingWorker::run_2", rtRess->dt->getCurrent() - start2);

        start2 = rtRess->dt->getCurrent();
        ColorLight colorL = ray.getColor();
        int red = colorL.red() * colorL.alpha() / 255;
        int green = colorL.green() * colorL.alpha() / 255;
        int blue = colorL.blue() * colorL.alpha() / 255;
        int light = colorL.getLight();

        light += RAYTRACING::gamma;

        totalLight += light;
        colors.replace(i, ColorLight(red, green, blue, 255, light));
        rtRess->dt->addValue("RayTracingWorker::run_3", rtRess->dt->getCurrent() - start2);
    }

    rtRess->dt->addValue("RayTracingWorker::run_colors", rtRess->dt->getCurrent() - start);
    emit resultReady(this, xScene, colors, totalLight);
}




RayTracing::RayTracing(const map3D* map) : QThread()
{
    this->map = map;
    rtRess = new RayTracingRessources(map->getWorld(), map->getClient(), &dt);

    connect(map->getWorld(), &World::changed, this, &RayTracing::worldChanged);

    rayWorkers = QList<RayTracingWorker*>(RAYTRACING::WorkerThread);
    for (int i = 0; i < rayWorkers.length(); i++) {
        rayWorkers[i] = new RayTracingWorker(i, rtRess);
        connect(rayWorkers[i], &RayTracingWorker::resultReady, this, &RayTracing::onRayWorkerReady);
    }
    image = QImage(1, 1, QImage::Format::Format_RGB32);
    image.fill(Qt::black);
}

RayTracing::~RayTracing()
{
    for (int i = 0; i < rayWorkers.length(); i++) rayWorkers[i]->quit();
    for (int i = 0; i < rayWorkers.length(); i++) {
        rayWorkers[i]->wait(1000);
        delete rayWorkers[i];
    }
    rayWorkers.clear();
}


void RayTracing::onRayWorkerReady(RayTracingWorker* worker, int x, const QList<ColorLight>& c, int totalLight)
{
    processDone++;
    lightPerColumn.replace(x, totalLight);
    colors.setColumn(x, c);

    if (processDone < processToStart) {
        assignNextRayWork(worker);
        if (processDone % RAYTRACING::RefreshColumn == 0) paint();
    }
    else {
        onAllWorkersFinished();
    }
}

void RayTracing::worldChanged(const WorldChange& change) {
    switch (change.type()) {
    case WorldChange::Type::block:
    case WorldChange::Type::chunk: {
        rtRess->worldChanged();
        break;
    }
    case WorldChange::Type::entity:
        break;
    }
}

void RayTracing::run() {
    dt.clear();
    startRun = dt.getCurrent();

    if (calcSize != colors.size()) {
        yPosList = RayTracingWorker::calcyPosList(calcSize.height());
        for (int i = 0; i < rayWorkers.length(); i++) {
            rayWorkers[i]->setPrimaryWork(yPosList, calcSize);
        }
        colors = PixScreen<ColorLight>(calcSize);
        lightPerColumn = QList<double>(calcSize.width());
        image = image.scaled(calcSize);
    }

    for (int x = 0; x < image.width(); x++) {
        image.setPixelColor(x, 0, Qt::white);
        image.setPixelColor(x, 1, Qt::white);
    }

    rtRess->resetRessources(map->getClient());// reset the pos

    processToStart = calcSize.width();
    processDone = 0;
    processStarted = 0;

    workersInProcess = true;
    for (int i = 0; i < rayWorkers.length() && i < processToStart; i++) {
        assignNextRayWork(rayWorkers[i]);
    }
    // then wait for the last onRayWorkerReady
    // N.B. this function takes less than 1 msec
}

void RayTracing::assignNextRayWork(RayTracingWorker* worker)
{
    if (processStarted >= processToStart || worker->isRunning()) return;
    worker->setWork(processStarted)->start();
    processStarted++;
}

double RayTracing::calcTotalLight() const
{
    double totalLight = 0;//une sorte de moyenne pour privilégier le poids des très lumineux
    for (int i = 0; i < lightPerColumn.length(); i++) totalLight += lightPerColumn.at(i);
    totalLight /= colors.rowNumber();//moyenne par pixel
    return totalLight;
    // full screen : 2 msec / 20 appels
}

void RayTracing::paint()
{
    qint64 start = dt.getCurrent();
    double totalLight = calcTotalLight();
    qDebug() << "RayTracing::paint #totalLight" << totalLight;

    for (int x = 0; x < colors.width() && x < processDone; x++) {
        for (int y = 0; y < colors.height(); y++) {
            ColorLight cl = colors.at(x, y);
            QColor color = cl.getColorReduced(totalLight);
            image.setPixelColor(x, y, color);
        }
    }

    dt.addValue("paint", dt.getCurrent() - start);
    emit resultReady(image);
}


void RayTracing::onAllWorkersFinished()
{
    qDebug() << "All ray workers have finished, processDone :" << processDone << "/" << processToStart;
    workersInProcess = false;
    paint();
    dt.addValue("run", dt.getCurrent() - startRun);
    qDebug() << "GUIWorker::run #end" << dt;
}
