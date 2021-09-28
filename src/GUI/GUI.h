#ifndef GUI_H
#define GUI_H

#include "../World/map3D.h"
#include "../RayTracing/RayTracing.h"

class GUI
{
public:
    GUI(const map3D *map);
    ~GUI();

    void refresh();
    inline bool isPainting() const { return workerThread->isRunning(); }
    void switchFPSCounterVisible();

    // signals:
    void workStarted();
    void workFinished();

private:
    // void paintEvent(QPaintEvent *event) override;
    void handleWorkerResults();
    // QSize getRayTracingSize() const;
    void onFPSTimeout();

    RayTracing *workerThread;
    const map3D *map;
    bool showFPSCounter = false;
    int previousFPS;
    int frameCounter;
    // QTimer timerFPS;
    const RayImage *rayImage;
};

#endif // GUI_H
