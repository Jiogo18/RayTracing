#include "GUI.h"

GUI::GUI(const map3D *map) : workerThread(new RayTracingCPU(map)), map(map), rayImage(workerThread->getImage())
{
    workerThread->connectResultReady([this]() { this->handleWorkerResults(); });
    previousFPS = 0;
    frameCounter = 0;
    frameCounterBegin = Timer::getCurrentMsSinceLocal();
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

    workerThread->setSize(getRayTracingSize())->start();
}

void GUI::switchFPSCounterVisible()
{
    showFPSCounter = !showFPSCounter;
}

void GUI::updateFPSCounter()
{
    int now = Timer::getCurrentMsSinceLocal();
    if (frameCounter != 0)
        previousFPS = roundf(frameCounter * 1000.0f / (now - frameCounterBegin) * 10) * 0.1f;
    else
        previousFPS = 0;
    previousFPSString = std::to_string(previousFPS);
    previousFPSString = previousFPSString.substr(0, previousFPSString.find('.') + 2);
    frameCounter = 0;
    frameCounterBegin = now;
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
void GUI::paintEvent()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glRasterPos2i(0, 0);
    glDrawPixels(rayImage->height(), rayImage->width(), GL_RGBA, GL_UNSIGNED_BYTE, rayImage->data);

    if (showFPSCounter) {
        // Draw the FPS at the top left
        glColor3f(1, 1, 1);
        glRasterPos2f(4, 4);
        for (char c : previousFPSString) {
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_10, c);
        }
    }

    glFlush();
}

// toute l'image a été actualisée (par le GUIWorker)
void GUI::handleWorkerResults()
{
    frameCounter++;
    // repaint();
    // if (!isPainting()) {
    workFinishedCallback();
    // }
}

SIZE GUI::getRayTracingSize() const
{
    return getWindowSize();
}
