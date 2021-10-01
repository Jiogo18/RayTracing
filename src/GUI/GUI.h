#ifndef GUI_H
#define GUI_H

#include "../World/map3D.h"
#include "../RayTracing/RayTracing.h"
#include "../Qt_compat/Timer.h"
#include <windows.h>
#include <wingdi.h>

class GUI
{
public:
    GUI(const map3D *map);
    ~GUI();

    void refresh();
    inline bool isPainting() const { return workerThread->isRunning(); }
    void switchFPSCounterVisible();

    void connectOnWorkStarted(std::function<void()> callback);
    void connectOnWorkFinished(std::function<void()> callback);

    void paintEvent(HWND hWnd);

private:
    void handleWorkerResults();
    virtual QSize getRayTracingSize() const = 0;
    void onFPSTimeout();

    RayTracing *workerThread;
    const map3D *map;
    bool showFPSCounter = false;
    int previousFPS;
    int frameCounter;
    Timer timerFPS;
    const RayImage *rayImage;

    std::function<void()> workStartedCallback;
    std::function<void()> workFinishedCallback;
};

#endif // GUI_H
