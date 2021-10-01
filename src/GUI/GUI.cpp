#include "GUI.h"

GUI::GUI(const map3D *map) : workerThread(new RayTracing(map)), map(map), rayImage(workerThread->getImage())
{

    // TODO
    workerThread->connectResultReady([this]() { this->handleWorkerResults(); });
    previousFPS = 0;
    frameCounter = 0;
    timerFPS.setInterval(1000);
    timerFPS.connectOnTimeout([this]() { this->onFPSTimeout(); });
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

    workStartedCallback();

    // workerThread->setSize(getRayTracingSize())->start();
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

void GUI::connectOnWorkStarted(std::function<void()> callback)
{
    workStartedCallback = callback;
}
void GUI::connectOnWorkFinished(std::function<void()> callback)
{
    workFinishedCallback = callback;
}

// garder l'image d'origine mais changer la taille de l'image à l'écran
// void GUI::paintEvent(QPaintEvent *event)
// {
//     // see 2D Painting Example (with OpenGL)
//     QPainter painter(this);
//     painter.setRenderHint(QPainter::Antialiasing);
//     painter.drawImage(0, 0, rayImage->toQImage(size()));
//     if (showFPSCounter) {
//         painter.setPen(Qt::white);
//         painter.drawText(0, 10, QString::number(previousFPS));
//     }
//     painter.end();
// }

// toute l'image a été actualisée (par le GUIWorker)
void GUI::handleWorkerResults()
{
    frameCounter++;
    // repaint();
    if (!isPainting()) {
        workFinishedCallback();
    }
}

// QSize GUI::getRayTracingSize() const
// {
//     return QSize(width() * RAYTRACING::pppH, height() * RAYTRACING::pppV);
// }

void GUI::onFPSTimeout()
{
    previousFPS = frameCounter;
    frameCounter = 0;
}
