#include "RayTracing.h"



template<typename T>
PixScreen<T>::PixScreen(const QSize& size)
{
    screen = QList<QList<T>>(size.width());
    for (int i = 0; i < size.width(); i++) {
        screen[i] = QList<T>(size.height());
    }
}


RayTracingRessources::RayTracingRessources(const World* world, const Pos3D& clientPos, DebugTime* dt)
{
    this->world = world; this->clientPos = clientPos;
    facesImg = new QMap<QString, const QImage*>;
    this->dt = dt;
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




Ray::Ray(Pos3D pos, RayTracingRessources* rtRess)
{
    this->rtRess = rtRess;
    origin = pos;
    distParcouru = 0;
    moveTo(pos);
    distParcouru = 0;
}

ColorLight Ray::getColor() const
{
    if (colors.isEmpty())
        return ColorLight(0, 0, 0, 255, 0);
    ColorLight retour = colors.last();
    for (int i = colors.size() - 2; i >= 0; i--)
        retour = colors.at(i) + retour;
    return retour;
}

void Ray::process(const World* world)
{
    while (distParcouru < viewDistance) {//pas exactement view distance mais pas loins (2*..?)
        qint64 start = rtRess->dt->getCurrent();
        Point3D pInter;
        const Face* face = getFirstIntersection(world, &pInter);
        rtRess->dt->addValue("Ray::process_face", rtRess->dt->getCurrent() - start);
        if (face == nullptr || !face->isValid())
            break;//c'est normal si on va dans le vide

        lastMoved = !(pInter == pos.getPoint());
        lastFace = face;

        const Plan* plan = face->getPlan();
        const QPointF pTexture = plan->projeteOrtho(pInter) - face->getPointC();
        colors.append(face->getColor(pTexture, getTexture(face->getTexture())));

        if (face->getMaterial() == BLOCK::Material::mirror) {
            moveTo(Pos3D(pInter, face->boundRotX(pos.getRX()), face->boundRotZ(pos.getRZ())));
        }
        else {
            float newSpeed = OBJECT3D::getSpeedOfLight(face->getMaterial());
            float previousSpeed = lastFaceIntersection ? OBJECT3D::getSpeedOfLight(lastFaceIntersection->getMaterial()) : 1;
            // calcul de la réfraction
            moveTo(Pos3D(pInter, face->refractRotX(pos.getRX(), previousSpeed, newSpeed), face->refractRotZ(pos.getRZ(), previousSpeed, newSpeed)));
            //moveTo(Pos3D(pInter, pos.getRX(), pos.getRZ()));
        }

        int alpha = colors.last().getColorA().alpha();
        opacity += alpha;
        if (opacity >= 255)
            break;//on s'arrete la
        //break;//TODO empecher de calculer après pour les tests (avoir que la première face)
        //Tips: transparant il faut penser à passer sur la face de l'autre coté... trop complexe :'(
        lastFaceIntersection = face;
    }
}

void Ray::moveTo(const Pos3D& pos)
{
    distParcouru += Point3D::distance(this->pos, pos);
    this->pos = pos;
    posNextPoint = pos.getNextPoint();//pour pas le recalculer pour toutes les faces
}

const Face* Ray::getFirstIntersection(const World* world, Point3D* pInter) const
{
    const Face* faceMin = nullptr;
    Point3D pInterMin;
    doubli distanceInterMin = 0;//seront set car !faceMin.isValid() au début
    doubli distanceBlockMin = 0;
    for (int i = 0; i < world->getChunks().size(); i++) {
        Chunk* chunk = world->getChunks().at(i);
        if (Point3D::distanceMax(chunk->getPoint(), pos) > viewDistance)
            continue;
        qint64 start1 = rtRess->dt->getCurrent();
        for (int i2 = 0; i2 < chunk->getBlocks()->size(); i2++) {
            Block* block = chunk->getBlocks()->at(i2);
            if (Point3D::distanceMax(block->getPoint(), pos) > viewDistance)
                continue;
            for (int i3 = 0; i3 < block->getFaces()->size(); i3++) {
                const Face* face = block->getFaces()->at(i3);
                if (face == lastFace)
                    continue;
                const Plan* plan = face->getPlan();

                if (!plan->isValid()) continue;

                Point3D pInter = plan->intersection(pos, posNextPoint);

                if (!pInter.isValid() || !face->getMaxGeometry().contains(pInter))
                    continue;
                if (!enter && pInter == pos.getPoint() && !lastMoved)
                    continue;//on viens de sortir (ou par défaut) et on se retrouve sur le même point
                doubli distanceInter = roundNull(Point3D::distance(pos, pInter)),
                    distanceBlock = 0;
                if (faceMin != nullptr && faceMin->isValid()) {
                    if (distanceInter > distanceInterMin)
                        continue;
                    distanceBlock = roundNull(Point3D::distance(pos, block->getMiddleGeometry()));
                    if (distanceInter == distanceInterMin) {
                        if (distanceBlock >= distanceBlockMin)
                            continue;
                    }
                }
                else {
                    distanceBlock = roundNull(Point3D::distance(pos, block->getMiddleGeometry()));
                }
                //si il n'y a pas encore de face
                //sinon si le pt d'intersection est plus proche
                //sinon si c'est le même et si la face est plus proche
                //sinon si c'est la même et si le block est plus proche
                //alors on prend
                faceMin = face;
                pInterMin = pInter;
                distanceInterMin = distanceInter;
                distanceBlockMin = distanceBlock;
                //il est possible qu'un même distance soit pour le meme block, pour des faces diff (de meme distance)
                //mais c'est pas grave car ça arrive quand c'est VRAIMENT indéterminant
                //(on peu prendre l'un ou l'autre)
            }

        }
        rtRess->dt->addValue("Ray::firstIntersection_1", rtRess->dt->getCurrent() - start1);

    }
    *pInter = pInterMin;
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
    radiant angleH = atan(xPos);
    for (int y = 0; y < sceneSize.height(); y++) {
        //pos en % de pixmap.height/2 * yMax
        doubli yPos = yPosList.at(y);
        doubli d = sqrt(sqr(xPos) + sqr(yPos) + 1);
        radiant angleV = asin(yPos / d);

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
        QColor colorA = colorL.getColorA();
        int red = colorA.red() * colorA.alpha() / 255;
        int green = colorA.green() * colorA.alpha() / 255;
        int blue = colorA.blue() * colorA.alpha() / 255;
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
    rtRess = new RayTracingRessources(map->getWorld(), map->getClient()->getPos(), &dt);

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

    rtRess->clientPos = map->getClient()->getPos();

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
