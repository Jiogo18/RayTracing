#ifndef FENETRE_H
#define FENETRE_H

#ifndef UNICODE
#define UNICODE
#endif // UNICODE

#include "GUI.h"
#include "../3D/Point2D.h"

// classe pour controller le GUI
// si un jour on veut retirer le contrôle sur le GUI (quand rafraichir, keyPress...)
// on aura juste à modifier cette classe

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
        right_rot = (1 << 9),
        ctrl = (1 << 10),
    };
    int getKeyAction(int key);
} // namespace KEY

class fenetre : public GUI
{
public:
    fenetre(map3D *map);
    ~fenetre();

    void keyPressEvent(int key, int status);
    void keyReleaseEvent(int key, int status);
    void updatePressPosition();
    // void mouseMoveEvent(QMouseEvent *event) override;
    // void showEvent(QShowEvent *event) override { Q_UNUSED(event) button->setWindow(this->windowHandle()); }
    // QPoint MidWindow();
    // void moveMouseMidWindow();
    void speedTest();
    void loadMapFile();

    int exec();

private:
    int window; // Identificateur de fenêtre
    SIZE getWindowSize() const;

    void onWorkStarted();
    void onWorkFinished();
    void onSpeedTestFinished();

    SingleShotTimer timerRefresh;
    int64_t lastRefreshTime;
    int64_t lastRefreshDuration;
    map3D *map = nullptr;

    // QPoint posMouse;
    doubli MouseSensibility = 2;

    int keysPressed = 0; // combinaison de keyAction

    bool testSpeedActivated = false;
    int testSpeedCounter;
    int64_t testSpeedTime;
};

void timerFPSCallback(int value);
void timerPositionCallback(int value);

#endif // FENETRE_H
