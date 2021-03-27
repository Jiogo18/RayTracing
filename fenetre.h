#ifndef FENETRE_H
#define FENETRE_H

#include "GUI.h"
#include "map3D.h"
#include <QTimer>
#include <QDebug>
#include <QKeyEvent>
#include <QPoint>

//classe pour controller le GUI
//si un jour on veut retirer le contrôle sur le GUI (quand rafraichir, keyPress...) on aura juste à modifier cette classe

namespace KEY {
    enum keyAction {
        forward,
        back,
        left,
        right,
        up,
        down,
        up_rot,
        down_rot,
        left_rot,
        right_rot
    };
    const QMap<int, keyAction> keyConfig = {
        { Qt::Key_Z, forward },
        { Qt::Key_S, back},
        { Qt::Key_Q, left},
        { Qt::Key_D, right },
        { Qt::Key_Space, up },
        { Qt::Key_Shift, down },
        { Qt::Key_Up, up_rot },
        { Qt::Key_Down, down_rot },
        { Qt::Key_Left, left_rot },
        { Qt::Key_Right, right_rot },
    };
}





class fenetre : public GUI
{
public:
    fenetre(map3D* map, QWidget* parent = nullptr);
    ~fenetre();

    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;
    //    void mouseMoveEvent(QMouseEvent *event) override;
        //void showEvent(QShowEvent *event) override { Q_UNUSED(event) button->setWindow(this->windowHandle()); }
    //    QPoint MidWindow();
    //    void moveMouseMidWindow();
    void speedTest();

private slots:
    void workStarted();
    void workFinished();
    void onSpeedTestFinished();

private:
    void updatePressPosition();
    QTimer timerRefresh;
    qint64 lastRefreshTime;
    qint64 lastRefreshDuration;
    map3D* map = nullptr;

    //    QPoint posMouse;
    doubli MouseSensibility = 2;

    QTimer timerKeyPress;
    int keysPressed = 0;// combinaison de keyAction

    bool testSpeedActivated = false;
    int testSpeedCounter;
    qint64 testSpeedTime;
};

#endif // FENETRE_H
