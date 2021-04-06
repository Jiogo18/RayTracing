#include "GUI.h"

GUI::GUI(const map3D *map, QWidget *parent) : QWidget(parent)
{
    this->map = map;

    workerThread = new RayTracing(map);
    connect(workerThread, &RayTracing::resultReady, this, &GUI::handleWorkerResults);

    lastImage = workerThread->getImage();
    previousFPS = 0;
    frameCounter = 0;
    timerFPS.setInterval(1000);
    connect(&timerFPS, &QTimer::timeout, this, &GUI::onFPSTimeout);
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

void GUI::switchFPSCounterVisible()
{
    showFPSCounter = !showFPSCounter;
    if (showFPSCounter) {
        timerFPS.start();
    } else {
        timerFPS.stop();
    }
}

// garder l'image d'origine mais changer la taille de l'image à l'écran
void GUI::paintEvent(QPaintEvent *event)
{
    // see 2D Painting Example (with OpenGL)
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.drawImage(0, 0, lastImage.scaled(size()));
    if (showFPSCounter) {
        painter.setPen(Qt::white);
        painter.drawText(0, 10, QString::number(previousFPS));
    }
    painter.end();
}

// toute l'image a été actualisée (par le GUIWorker)
void GUI::handleWorkerResults(const QImage &image)
{
    lastImage = image;
    frameCounter++;
    repaint();
    if (!isPainting()) {
        emit workFinished();
    }
}

// pourquoi je m'embete avec des ppp quand je peux juste faire un scale down ?
QSize GUI::getRayTracingSize() const
{
    return QSize(width() * RAYTRACING::pppH, height() * RAYTRACING::pppV);
}

void GUI::onFPSTimeout()
{
    previousFPS = frameCounter;
    frameCounter = 0;
}
