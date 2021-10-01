#include "GUI.h"

GUI::GUI(const map3D *map) : workerThread(new RayTracing(map)), map(map), rayImage(workerThread->getImage())
{

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

void GUI::connectOnWorkStarted(std::function<void()> callback)
{
    workStartedCallback = callback;
}
void GUI::connectOnWorkFinished(std::function<void()> callback)
{
    workFinishedCallback = callback;
}

// garder l'image d'origine mais changer la taille de l'image à l'écran
void GUI::paintEvent(HWND hWnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    // see 2D Painting Example (with OpenGL)
    int width = ps.rcPaint.right - ps.rcPaint.left;
    int height = ps.rcPaint.bottom - ps.rcPaint.top;
    Image image = rayImage->toImage();
    for (int y = ps.rcPaint.top; y < ps.rcPaint.bottom; y++) {
        for (int x = ps.rcPaint.left; x < ps.rcPaint.right; x++) {
            SetPixel(hdc, x, y, image.pixelRGB(x, y));
        }
    }
    if (showFPSCounter) {
        // painter.setPen(Qt::white);
        std::string textFPS = std::to_string(previousFPS);
        LPCSTR textFPS2 = textFPS.c_str();
        RECT textFPSRect = {0, 10, 50, 30};
        DrawText(hdc, (LPCTSTR)textFPS2, textFPS.length(), &textFPSRect, DT_LEFT);
    }

    EndPaint(hWnd, &ps);
}

// toute l'image a été actualisée (par le GUIWorker)
void GUI::handleWorkerResults()
{
    frameCounter++;
    // repaint();
    if (!isPainting()) {
        workFinishedCallback();
    }
}

void GUI::onFPSTimeout()
{
    previousFPS = frameCounter;
    frameCounter = 0;
}
