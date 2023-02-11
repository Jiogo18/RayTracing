#ifndef GUI_H
#define GUI_H

#include "../World/map3D.h"
#include "../RayTracing/RayTracing.h"
#include "../utils/Timer.h"
#include "winapi.h"

class GUI
{
public:
    GUI(const map3D *map);
    ~GUI();

    void refresh();
    inline bool isPainting() const { return workerThread->isRunning(); }
    void switchFPSCounterVisible();
    void updateFPSCounter();

    void connectOnWorkStarted(std::function<void()> callback);
    void connectOnWorkFinished(std::function<void()> callback);

    void paintEvent(HWND hWnd);

private:
    void handleWorkerResults();
    virtual SIZE getWindowSize() const = 0;
    SIZE getRayTracingSize() const;

    RayTracing *workerThread;
    const map3D *map;
    bool showFPSCounter = false;
    int previousFPS;
    int frameCounter;
    const Image *rayImage;

    HBITMAP hBitmap = NULL;
    BYTE *hBmpPixels = nullptr;
    BITMAPINFO MyBMInfo = {0};

    std::function<void()> workStartedCallback;
    std::function<void()> workFinishedCallback;
};

#endif // GUI_H
