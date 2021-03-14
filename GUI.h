#ifndef GUI_H
#define GUI_H

#include "map3D.h"
#include "RayTracing.h"
#include <QThread>
#include <QRandomGenerator>
#include <QWidget>
#include <QPaintEvent>


class GUI : public QObject
{
    Q_OBJECT
public:
    GUI(const map3D *map, QWidget *parent = nullptr);
    ~GUI();

    void paint(QPainter *painter, QPaintEvent *event);
    void refresh();


signals:
    void workStarted();
    void workReady();

private:
    void handleWorkerResults(const QImage &image);
    QSize getRayTracingSize() const;

    QWidget *parent = nullptr;
    QImage lastImage;
    RayTracing *workerThread;
    const map3D *map;
};

#endif // GUI_H
