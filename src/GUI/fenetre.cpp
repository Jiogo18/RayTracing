#include "fenetre.h"

#define PIXELS_WIDTH 150
#define PIXELS_HEIGHT 100
#define WINDOW_WIDTH (PIXELS_WIDTH + 15)
#define WINDOW_HEIGHT (PIXELS_HEIGHT + 38)

// 25 FPS max (vitesse de déplacement & de rafraichissement auto)
#define POSITION_REFRESH_TIME 40

fenetre *instance = nullptr;

int KEY::getKeyAction(int key)
{
    switch (key) {
    case 'Z':
        return KEY::keyAction::forward;
    case 'Q':
        return KEY::keyAction::left;
    case 'S':
        return KEY::keyAction::back;
    case 'D':
        return KEY::keyAction::right;
    case VK_SPACE:
        return KEY::keyAction::up;
    case VK_SHIFT:
        return KEY::keyAction::down;
    case VK_UP:
        return KEY::keyAction::up_rot;
    case VK_LEFT:
        return KEY::keyAction::left_rot;
    case VK_DOWN:
        return KEY::keyAction::down_rot;
    case VK_RIGHT:
        return KEY::keyAction::right_rot;
    default:
        return 0;
    }
}

fenetre::fenetre(map3D *map) : GUI(map), map(map)
{
    instance = this;
    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    window = glutCreateWindow("RayTracing");

    glClearColor(0, 0, 0, 0);
    gluOrtho2D(0, PIXELS_WIDTH, 0, PIXELS_HEIGHT); // On garde ces coordonnées

    glutDisplayFunc([]() { instance->paintEvent(); }); // Callback de la fenêtre
    glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);
    glutKeyboardFunc([](unsigned char key, int x, int y) { instance->keyPressEvent(key, 0); }); // Callback du clavier
    glutKeyboardUpFunc([](unsigned char key, int x, int y) { instance->keyReleaseEvent(key, 0); });
    glutSpecialFunc([](int key, int x, int y) { instance->keyPressEvent(key, 1); }); // Callback du clavier
    glutSpecialUpFunc([](int key, int x, int y) { instance->keyReleaseEvent(key, 1); });

    // std::cout << "[WinMain] PaintWindow Timer #" << SetTimer(hWnd, 0, 50, (TIMERPROC)TimerProc) << " created! (20 FPS)" << std::endl;

    connectOnWorkStarted([this]() { this->onWorkStarted(); });
    connectOnWorkFinished([this]() { this->onWorkFinished(); });
    // setCursor(Qt::BlankCursor);

    // moveMouseMidWindow();
    // posMouse = QPoint(0, 0);

    lastRefreshTime = 0;
    refresh();

    glutTimerFunc(1000, timerFPSCallback, 0);
    glutTimerFunc(POSITION_REFRESH_TIME, timerPositionCallback, 0);
    timerRefresh.connectOnFinished([this]() { this->refresh(); });
}

fenetre::~fenetre()
{
    instance = nullptr;
}

void fenetre::keyPressEvent(int key, int status)
{
    if (status == 0) {
        key = toupper(key);
        switch (key) {
        case 'Z':
        case 'Q':
        case 'S':
        case 'D':
        case VK_SPACE:
            keysPressed |= KEY::getKeyAction(key);
            break;
        }
    } else {
        switch (key) {
        case GLUT_KEY_UP:
            keysPressed |= KEY::getKeyAction(VK_UP);
            break;
        case GLUT_KEY_LEFT:
            keysPressed |= KEY::getKeyAction(VK_LEFT);
            break;
        case GLUT_KEY_DOWN:
            keysPressed |= KEY::getKeyAction(VK_DOWN);
            break;
        case GLUT_KEY_RIGHT:
            keysPressed |= KEY::getKeyAction(VK_RIGHT);
            break;
        case 0x70: // Left Shift
            keysPressed |= KEY::getKeyAction(VK_SHIFT);
            break;
        case GLUT_KEY_F5:
            refresh();
            break;
        case GLUT_KEY_F6:
            switchFPSCounterVisible();
            break;
        case GLUT_KEY_F7:
            speedTest();
            break;
        case GLUT_KEY_F8:
            loadMapFile();
            break;
        }
    }
}

void fenetre::keyReleaseEvent(int key, int status)
{
    int kA;
    if (status == 0) {
        kA = KEY::getKeyAction(toupper(key));
    } else {
        switch (key) {
        case GLUT_KEY_UP:
            kA = KEY::getKeyAction(VK_UP);
            break;
        case GLUT_KEY_LEFT:
            kA = KEY::getKeyAction(VK_LEFT);
            break;
        case GLUT_KEY_DOWN:
            kA = KEY::getKeyAction(VK_DOWN);
            break;
        case GLUT_KEY_RIGHT:
            kA = KEY::getKeyAction(VK_RIGHT);
            break;
        case 0x70: // Left Shift
            kA = KEY::getKeyAction(VK_SHIFT);
            break;
        default:
            return;
        }
    }

    if (keysPressed & kA)
        keysPressed ^= kA; // remove the key
}

void fenetre::speedTest()
{
    if (testSpeedActivated) return;
    testSpeedActivated = true;
    testSpeedCounter = 0;
    testSpeedTime = DebugTime::getCurrent();
    std::cout << "speedTest started for 1000 msec" << std::endl;
    refresh();
}

void fenetre::loadMapFile()
{
    std::string mapName = "./build/maps/Perlin3D_32_2.txt";
    // std::string mapName = "./build/maps/Perlin3D_8.txt";
    // std::string mapName = "./build/maps/JL.txt";
    if (map->load(mapName)) {
        std::cout << "successfully reloaded !" << std::endl;
    } else {
        std::cout << "Can't load the map \"" << mapName << "\"" << std::endl;
    }
    refresh();
}

int fenetre::exec()
{
    // Main loop
    glutMainLoop();
    return 0;
}

// void fenetre::mouseMoveEvent(QMouseEvent *event)
//{
//     Q_UNUSED(event)
//     //map->getClient()->moveRX((MidWindow().x() - event->x())/MouseSensibility);
//     //map->getClient()->moveRY((MidWindow().y() - event->y())/MouseSensibility);
//     //moveMouseMidWindow();
// }

// QPoint fenetre::MidWindow() { return QPoint(width()/2, height()/2); }
// void fenetre::moveMouseMidWindow() { QCursor::setPos(MidWindow() + QWidget::pos()); }

SIZE fenetre::getWindowSize() const
{
    return {glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT)};
}

void fenetre::onWorkStarted()
{
    lastRefreshTime = DebugTime::getCurrent();
    // setCursor(Qt::WaitCursor);
}
void fenetre::onWorkFinished()
{
    // setCursor(Qt::ArrowCursor);

    glutPostRedisplay(); // call paintEvent

    lastRefreshDuration = DebugTime::getCurrent() - lastRefreshTime;
    if (testSpeedActivated) onSpeedTestFinished();
}

void fenetre::onSpeedTestFinished()
{
    testSpeedCounter++;
    int64_t duration = (DebugTime::getCurrent() - testSpeedTime) / 1000;
    if (duration >= 2000) {
        // end
        std::cout << "speedTest with " << testSpeedCounter << " refresh in "
                  << duration << " msec ("
                  << (duration / testSpeedCounter) << " msec/frame, "
                  << ((float)testSpeedCounter / duration * 1000) << " FPS)" << std::endl;
        testSpeedActivated = false;
    } else
        refresh();
}

void fenetre::updatePressPosition()
{
    if (keysPressed == 0) return;
    if (keysPressed & KEY::keyAction::forward) map->moveFront();
    if (keysPressed & KEY::keyAction::back) map->moveBack();
    if (keysPressed & KEY::keyAction::left) map->moveLeft();
    if (keysPressed & KEY::keyAction::right) map->moveRight();
    if (keysPressed & KEY::keyAction::up) map->moveUp();
    if (keysPressed & KEY::keyAction::down) map->moveDown();
    if (keysPressed & KEY::keyAction::up_rot) map->moveRZ(MouseSensibility / 30);
    if (keysPressed & KEY::keyAction::down_rot) map->moveRZ(-MouseSensibility / 30);
    if (keysPressed & KEY::keyAction::left_rot) map->moveRX(-MouseSensibility / 30);
    if (keysPressed & KEY::keyAction::right_rot) map->moveRX(MouseSensibility / 30);

    // std::cout << "pos client " << map->getClient()->getPos() << std::endl;
    if (isPainting()) {
        // actu à la fin du repaint en cours
        timerRefresh.start(lastRefreshDuration);
    } else {
        refresh();
    }
}

void timerFPSCallback(int value)
{
    instance->updateFPSCounter();
    glutTimerFunc(1000, timerFPSCallback, 0);
}

void timerPositionCallback(int value)
{
    instance->updatePressPosition();
    glutTimerFunc(POSITION_REFRESH_TIME, timerPositionCallback, 0);
}
