#ifndef GUI_H
#define GUI_H

#include "map3D.h"
#include "RayTracing.h"
#include <QWidget>
#include <QPaintEvent>

class GUI : public QWidget
{
    Q_OBJECT
public:
    GUI(const map3D *map, QWidget *parent = nullptr);
    ~GUI();

    void refresh();
    inline bool isPainting() const { return workerThread->isRunning(); }

signals:
    void workStarted();
    void workFinished();

private:
    void paintEvent(QPaintEvent *event) override;
    void handleWorkerResults(const QImage &image);
    QSize getRayTracingSize() const;

    QImage lastImage;
    RayTracing *workerThread;
    const map3D *map;
};

#endif // GUI_H
