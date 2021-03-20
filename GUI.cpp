#include "GUI.h"


GUI::GUI(const map3D* map, QWidget* parent) : QWidget(parent)
{
    this->map = map;

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


// actualiser toute l'image (pour le GUIWorker)
void GUI::refresh()
{
    if (isPainting()) return;

    emit workStarted();

    workerThread->setSize(getRayTracingSize())->start();
}

// garder l'image d'origine mais changer la taille de l'image à l'écran
void GUI::paintEvent(QPaintEvent* event)
{
    // see 2D Painting Example (with OpenGL)
    Q_UNUSED(event)
        QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawImage(0, 0, lastImage.scaled(size()));
    painter.end();
}

// toute l'image a été actualisée (par le GUIWorker)
void GUI::handleWorkerResults(const QImage& image)
{
    lastImage = image;
    repaint();
    if (!isPainting()) {
        emit workFinished();
    }
}

//pourquoi je m'embete avec des ppp quand je peux juste faire un scale down ?
QSize GUI::getRayTracingSize() const
{
    return QSize(width() * RAYTRACING::pppH, height() * RAYTRACING::pppV);
}
