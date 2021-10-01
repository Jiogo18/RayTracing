#ifndef FENETRE_H
#define FENETRE_H

#ifndef UNICODE
#define UNICODE
#endif // UNICODE

#include "GUI.h"
#include <windows.h>
#include <wingdi.h>
#include "../3D/Point2D.h"

// classe pour controller le GUI
// si un jour on veut retirer le contrôle sur le GUI (quand rafraichir, keyPress...)
// on aura juste à modifier cette classe

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
    // const std::vector<std::pair<int, keyAction>> keyConfig = {
    //     std::pair<int, keyAction>(Qt::Key_Z, forward),
    //     {Qt::Key_S, back},
    //     {Qt::Key_Q, left},
    //     {Qt::Key_D, right},
    //     {Qt::Key_Space, up},
    //     {Qt::Key_Shift, down},
    //     {Qt::Key_Up, up_rot},
    //     {Qt::Key_Down, down_rot},
    //     {Qt::Key_Left, left_rot},
    //     {Qt::Key_Right, right_rot},
    // };
} // namespace KEY

class fenetre : public GUI
{
public:
    fenetre(HINSTANCE hInstance, map3D *map);
    ~fenetre();

    void keyPressEvent(int key, int status);
    void keyReleaseEvent(int key, int status);
    //void mouseMoveEvent(QMouseEvent *event) override;
    //void showEvent(QShowEvent *event) override { Q_UNUSED(event) button->setWindow(this->windowHandle()); }
    //QPoint MidWindow();
    //void moveMouseMidWindow();
    void speedTest();
    void loadMapFile();

    WPARAM exec();
    void show(int nShowCmd);

private:
    HWND hWnd;
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    QSize getRayTracingSize() const;
    static fenetre *getFenetreFromHWND(HWND hWnd);

    void onWorkStarted();
    void onWorkFinished();
    void onSpeedTestFinished();

    void updatePressPosition();
    Timer timerRefresh;
    int64_t lastRefreshTime;
    int64_t lastRefreshDuration;
    map3D *map = nullptr;

    //QPoint posMouse;
    doubli MouseSensibility = 2;

    // QTimer timerKeyPress;
    int keysPressed = 0; // combinaison de keyAction

    bool testSpeedActivated = false;
    int testSpeedCounter;
    int64_t testSpeedTime;
};

#endif // FENETRE_H
