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
    for (int iChunk = 0; iChunk < world->getChunks().size(); iChunk++) {
        const Chunk* chunk = world->getChunks().at(iChunk);
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
    totalLight = new int[RAYTRACING::ColumnsPerWorker];
    colors = new QList<ColorLight>[RAYTRACING::ColumnsPerWorker];
}

RayTracingWorker::~RayTracingWorker()
{
    delete[] totalLight;
    delete[] colors;
}

RayTracingWorker* RayTracingWorker::setPrimaryWork(const QSize& sceneSize)
{
    this->sceneSize = sceneSize;
    return this;
}




QList<Pos3D> RayTracingWorker::calcRaysPosColumn(const doubli& xPos, const int& sceneHeight, const Pos3D& clientPos)
{
    QList<Pos3D> raysPos(sceneHeight);

    radian angleH = atan(xPos);
    for (int y = 0; y < sceneHeight; y++) {
        //pos en % de pixmap.height/2 * yMax
        doubli yPos = roundNull(1 - 2.0L * y / sceneHeight) * yMax;
        raysPos.replace(y, clientPos.getChildRot(angleH, atan(yPos / sqrt(xPos * xPos + 1))));
    }

    return raysPos;
}

void RayTracingWorker::run() {
    int i;
    for (i = 0; i < getNbColumns(); i++) {
        qint64 start;
        //start = rtRess->dt->getCurrent();
        totalLight[i] = 0;
        colors[i] = QList<ColorLight>(sceneSize.height());
        //pos en % de pixmap.width/2 * xMax
        doubli xPos = (2.0L * (xScene + i) / sceneSize.width() - 1) * xMax, xzPos = sqrt(xPos * xPos + 1);
        radian angleH = atan(xPos);
        //rtRess->dt->addValue("RayTracingWorker::run_init", rtRess->dt->getCurrent() - start);//1ms

        start = rtRess->dt->getCurrent();
        for (int y = 0; y < sceneSize.height(); y++) {
            qint64 start2;

            //start2 = rtRess->dt->getCurrent();
            //pos en % de pixmap.height/2 * yMax
            doubli yPos = (1 - 2.0L * y / sceneSize.height()) * yMax;
            Ray ray(rtRess->clientPos.getChildRot(angleH, atan(yPos / xzPos)), rtRess);
            //rtRess->dt->addValue("RayTracingWorker::run_1_Ray::Ray", rtRess->dt->getCurrent() - start2);//1080ms

#ifndef DISABLE_RAYPROCESS
            start2 = rtRess->dt->getCurrent();
            ray.process(rtRess->world);
            rtRess->dt->addValue("RayTracingWorker::run_2_process", rtRess->dt->getCurrent() - start2);
#endif // DISABLE_RAYPROCESS

            //start2 = rtRess->dt->getCurrent();
            ColorLight colorL = ray.getColor();
            int red = colorL.red() * colorL.alpha() / 255;
            int green = colorL.green() * colorL.alpha() / 255;
            int blue = colorL.blue() * colorL.alpha() / 255;
            int light = colorL.getLight();

            light += RAYTRACING::gamma;

            totalLight[i] += light;
            colors[i].replace(y, ColorLight(red, green, blue, 255, light));
            //rtRess->dt->addValue("RayTracingWorker::run_3_ColorLight", rtRess->dt->getCurrent() - start2);//150ms
        }
        rtRess->dt->addValue("RayTracingWorker::run_colors", rtRess->dt->getCurrent() - start);
    }

    emit resultReady(xScene, i, colors, totalLight);
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
        connect(rayWorkers[i], &QThread::finished, this, &RayTracing::onRayWorkerFinished);
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


void RayTracing::onRayWorkerReady(int x, int nbColumns, const QList<ColorLight>* c, const int* totalLight)
{
    processFinished += nbColumns;
    for (int i = 0; i < nbColumns; i++) {
        lightPerColumn.replace(x + i, totalLight[i]);
        colors.setColumn(x + i, c[i]);
    }

    if (processFinished >= processWidth) {
        onAllWorkersFinished();
    }
#ifdef REFRESH_COLUMN
    else {
        if (processForUpdate <= processFinished) {
            paint();
            processForUpdate = processFinished + RAYTRACING::RefreshColumn;
        }
    }
#endif // REFRESH_COLUMN
}

void RayTracing::onRayWorkerFinished()
{
    assignNextRayWork(qobject_cast<RayTracingWorker*>(sender()));
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
        for (int i = 0; i < rayWorkers.length(); i++) {
            rayWorkers[i]->setPrimaryWork(calcSize);
        }
        colors = PixScreen<ColorLight>(calcSize);
        lightPerColumn = QList<int>(calcSize.width());
        image = image.scaled(calcSize);
    }

    for (int x = 0; x < image.width(); x++) {
        image.setPixelColor(x, 0, Qt::white);
        image.setPixelColor(x, 1, Qt::white);
    }

    rtRess->resetRessources(map->getClient());// reset the pos

    processWidth = calcSize.width();
    processFinished = 0;
    processStarted = 0;
    processForUpdate = RAYTRACING::RefreshColumn;

    workersInProcess = true;
    for (int i = 0; i < rayWorkers.length() && i < processWidth; i++) {
        assignNextRayWork(rayWorkers[i]);
    }
    // then wait for the last onRayWorkerReady
    // N.B. this function takes less than 1 msec
}

void RayTracing::assignNextRayWork(RayTracingWorker* worker)
{
    if (worker->isRunning()) {
        qWarning() << "RayTracingWorker" << worker->getWorkerId() << "is already running";
        worker->wait(1);
        if (worker->isRunning()) {
            qWarning() << "RayTracingWorker" << worker->getWorkerId() << "skipped";
            return;
        }
    }
    if (processStarted >= processWidth) return;
    worker->setWork(processStarted)->start();
    processStarted += worker->getNbColumns();
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

    for (int x = 0; x < colors.width() && x < processFinished; x++) {
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
    qDebug() << "All ray workers have finished, processFinished :" << processFinished << "/" << processWidth;
    workersInProcess = false;
    paint();
    dt.addValue("run", dt.getCurrent() - startRun);
    qDebug() << "GUIWorker::run #end" << dt;
}
