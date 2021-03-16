#include "GUI.h"


GUI::GUI(const map3D *map, QWidget *parent) : QObject(parent)
{
    this->map = map;
    this->parent = parent;

    workerThread = new RayTracing(map);
    connect(workerThread, &RayTracing::resultReady, this, &GUI::handleWorkerResults);

    lastImage = workerThread->getImage();
}

GUI::~GUI()
{
    workerThread->quit();
    workerThread->wait(5000);
    delete workerThread;
}

// garder l'image d'origine mais changer la taille de l'image à l'écran
void GUI::paint(QPainter *painter, QPaintEvent *event)
{
    painter->drawImage(event->rect(), lastImage.scaled(event->rect().size()));
}


// actualiser toute l'image (pour le GUIWorker)
void GUI::refresh()
{
    if(workerThread->isRunning()) return;

    emit workStarted();

    workerThread->setSize(getRayTracingSize())->start();
}


// toute l'image a été actualisée (par le GUIWorker)
void GUI::handleWorkerResults(const QImage &image)
{
    lastImage = image;
    emit workReady();
    if(!workerThread->isRunning()) {
        emit workFinished();
    }
}

//pourquoi je m'embete avec des ppp quand je peux juste faire un scale down ?
QSize GUI::getRayTracingSize() const
{
    return QSize(parent->width() * RAYTRACING::pppH, parent->height() * RAYTRACING::pppV);
}
