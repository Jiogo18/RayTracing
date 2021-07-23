#ifndef GUI_H
#define GUI_H

#include "src/World/map3D.h"
#include "src/RayTracing/RayTracing.h"
#include <QWidget>
#include <QPaintEvent>
#include <QPainter>
#include <QTimer>

class GUI : public QWidget
{
    Q_OBJECT
public:
    GUI(const map3D *map, QWidget *parent = nullptr);
    ~GUI();

    void refresh();
    inline bool isPainting() const { return workerThread->isRunning(); }
    void switchFPSCounterVisible();

signals:
    void workStarted();
    void workFinished();

private:
    void paintEvent(QPaintEvent *event) override;
    void handleWorkerResults();
    QSize getRayTracingSize() const;
    void onFPSTimeout();

    RayTracing *workerThread;
    const map3D *map;
    bool showFPSCounter = false;
    int previousFPS;
    int frameCounter;
    QTimer timerFPS;
    const RayImage *rayImage;
};

#endif // GUI_H
