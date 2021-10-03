#include "fenetre.h"

#define PIXELS_WIDTH 150
#define PIXELS_HEIGHT 100

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

fenetre::fenetre(HINSTANCE hInstance, map3D *map) : GUI(map), map(map)
{
    // Register the window class.
    const wchar_t CLASS_NAME[] = L"Sample Window Class";
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClass(&wc);

    // Create the window.
    hWnd = CreateWindowEx(
        0,                   // Optional window styles.
        CLASS_NAME,          // Window class
        L"RayTracing",       // Window text
        WS_OVERLAPPEDWINDOW, // Window style

        // Position
        CW_USEDEFAULT, CW_USEDEFAULT,
        // Size
        PIXELS_WIDTH + 15, PIXELS_HEIGHT + 38,

        NULL,      // Parent window
        NULL,      // Menu
        hInstance, // Instance handle
        NULL       // Additional application data
    );

    if (hWnd == NULL) {
        throw "Failed to create WingdiWindow";
    }

    SetWindowLongPtr(hWnd, GWL_USERDATA, reinterpret_cast<LONG_PTR>(this));

    // std::cout << "[WinMain] PaintWindow Timer #" << SetTimer(hWnd, 0, 50, (TIMERPROC)TimerProc) << " created! (20 FPS)" << std::endl;

    connectOnWorkStarted([this]() { this->onWorkStarted(); });
    connectOnWorkFinished([this]() { this->onWorkFinished(); });
    //setCursor(Qt::BlankCursor);

    // show();
    // setMinimumSize(50, 50);
    // setGeometry(QRect(geometry().topLeft(), QSize(150, 100)));

    //moveMouseMidWindow();
    //posMouse = QPoint(0, 0);

    lastRefreshTime = 0;
    refresh();

    timerRefresh.connectOnFinished([this]() { this->refresh(); });

#ifdef UOI_TIMERPROC_EXCEPTION_SUPPRESSION
    BOOL bFalse = false;
    SetUserObjectInformation(GetCurrentProcess(), UOI_TIMERPROC_EXCEPTION_SUPPRESSION, &bFalse, sizeof(bool));
#endif
    SetTimer(hWnd, TIMER_POSITION, 40, (TIMERPROC)TimerPositionProc); // 25 FPS max (vitesse de déplacement & de rafraichissement auto)
    SetTimer(hWnd, TIMER_FPS, 1000, (TIMERPROC)TimerFPSProc);
}

fenetre::~fenetre() {}

void fenetre::keyPressEvent(int key, int status)
{
    if (status & 0x4000000) return; // auto repeat
    switch (key) {
    case VK_F5:
        refresh();
        break;
    case VK_F6:
        switchFPSCounterVisible();
        break;
    case VK_F7:
        speedTest();
        break;
    case VK_F8:
        loadMapFile();
        break;
    case 'Z':
    case 'Q':
    case 'S':
    case 'D':
    case VK_SPACE:
    case VK_SHIFT:
    case VK_UP:
    case VK_LEFT:
    case VK_DOWN:
    case VK_RIGHT:
        keysPressed |= KEY::getKeyAction(key);
        break;
    }
}

void fenetre::keyReleaseEvent(int key, int status)
{
    if (status == 0x4000000) return; // auto repeat

    int kA = KEY::getKeyAction(key);
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
    std::string mapName = "Perlin3D_32_2.txt";
    if (map->load(mapName)) {
        std::cout << "successfully reloaded !" << std::endl;
    } else {
        std::cout << "Can't load the map \"" << mapName << "\"" << std::endl;
    }
    refresh();
}

WPARAM fenetre::exec()
{
    // Run the message loop.
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return msg.wParam;
}

void fenetre::show(int nShowCmd)
{
    ShowWindow(hWnd, nShowCmd);
}

//void fenetre::mouseMoveEvent(QMouseEvent *event)
//{
//    Q_UNUSED(event)
//    //map->getClient()->moveRX((MidWindow().x() - event->x())/MouseSensibility);
//    //map->getClient()->moveRY((MidWindow().y() - event->y())/MouseSensibility);
//    //moveMouseMidWindow();
//}

//QPoint fenetre::MidWindow() { return QPoint(width()/2, height()/2); }
//void fenetre::moveMouseMidWindow() { QCursor::setPos(MidWindow() + QWidget::pos()); }

QSize fenetre::getWindowSize() const
{
    RECT rect;
    GetWindowRect(hWnd, &rect);
    return QSize(rect.right - rect.left - 15, rect.bottom - rect.top - 38);
}

fenetre *fenetre::getFenetreFromHWND(HWND hWnd)
{
    return reinterpret_cast<fenetre *>(GetWindowLongPtr(hWnd, GWL_USERDATA));
}

LRESULT CALLBACK fenetre::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        exit(0);
        return 0;

    case WM_PAINT: {
        fenetre *pThis = fenetre::getFenetreFromHWND(hWnd);
        pThis->paintEvent(hWnd);
        return 0;
    }
    case WM_KEYDOWN: {
        fenetre *pThis = fenetre::getFenetreFromHWND(hWnd);
        pThis->keyPressEvent(wParam, lParam);
        break;
    }
    case WM_KEYUP: {
        fenetre *pThis = fenetre::getFenetreFromHWND(hWnd);
        pThis->keyReleaseEvent(wParam, lParam);
        break;
    }
    default:
        // Process other messages.
        // std::cout << "[WindowProc/" << uMsg << "] " << wParam << " " << lParam << std::endl;
        break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void fenetre::TimerPositionProc(HWND hWnd, INT unnamedParam2, UINT_PTR unnamedParam3, DWORD localTimestamp)
{
    fenetre *pThis = fenetre::getFenetreFromHWND(hWnd);
    pThis->updatePressPosition();
}

void fenetre::TimerFPSProc(HWND hWnd, INT unnamedParam2, UINT_PTR unnamedParam3, DWORD localTimestamp)
{
    fenetre *pThis = fenetre::getFenetreFromHWND(hWnd);
    pThis->updateFPSCounter();
}

void fenetre::onWorkStarted()
{
    lastRefreshTime = DebugTime::getCurrent();
    // setCursor(Qt::WaitCursor);
}
void fenetre::onWorkFinished()
{
    // setCursor(Qt::ArrowCursor);
    RECT rcPaint{0, 0, PIXELS_WIDTH, PIXELS_HEIGHT};
    InvalidateRect(hWnd, &rcPaint, false);
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

    //std::cout << "pos client " << map->getClient()->getPos() << std::endl;
    if (isPainting()) {
        // actu à la fin du repaint en cours
        timerRefresh.start(lastRefreshDuration);
    } else {
        refresh();
    }
}
