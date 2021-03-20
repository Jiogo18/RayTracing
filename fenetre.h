#ifndef FENETRE_H
#define FENETRE_H

#include "GUI.h"
#include "map3D.h"
#include <QTimer>
#include <QDebug>
#include <QKeyEvent>
#include <QPoint>
//#include <QWinTaskbarButton>//need 'winextras' removed in Qt 6.0.0
//#include <QWinTaskbarProgress>

//classe pour controller le GUI
//si un jour on veut retirer le contrôle sur le GUI (quand rafraichir, keyPress...) on aura juste à modifier cette classe

class fenetre : public GUI
{
public:
    fenetre(map3D* map, QWidget* parent = nullptr);
    ~fenetre();

    void keyPressEvent(QKeyEvent* event) override;
    //    void mouseMoveEvent(QMouseEvent *event) override;
        //void showEvent(QShowEvent *event) override { Q_UNUSED(event) button->setWindow(this->windowHandle()); }
    //    QPoint MidWindow();
    //    void moveMouseMidWindow();

private slots:
    void workStarted();
    void workFinished();
    //    void setPBMax(int max);
    //    void setPBValue(int value);

private:
    QTimer timerRefresh;
    qint64 lastRefreshTime;
    qint64 lastRefreshDuration;
    map3D* map = nullptr;

    //QWinTaskbarButton *winTaskbarButt = nullptr;
//    QPoint posMouse;
    doubli MouseSensibility = 2;
};

#endif // FENETRE_H
