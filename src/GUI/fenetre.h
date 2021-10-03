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

#define TIMER_POSITION 0
#define TIMER_FPS 1

namespace KEY {
    enum keyAction {
        none = 0,
        forward = (1 << 0),
        back = (1 << 1),
        left = (1 << 2),
        right = (1 << 3),
        up = (1 << 4),
        down = (1 << 5),
        up_rot = (1 << 6),
        down_rot = (1 << 7),
        left_rot = (1 << 8),
        right_rot = (1 << 9)
    };
    int getKeyAction(int key);
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
    QSize getWindowSize() const;
    static fenetre *getFenetreFromHWND(HWND hWnd);
    static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    static void TimerPositionProc(HWND hWnd, INT unnamedParam2, UINT_PTR unnamedParam3, DWORD localTimestamp);
    static void TimerFPSProc(HWND hWnd, INT unnamedParam2, UINT_PTR unnamedParam3, DWORD localTimestamp);

    void onWorkStarted();
    void onWorkFinished();
    void onSpeedTestFinished();

    void updatePressPosition();
    SingleShotTimer timerRefresh;
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
