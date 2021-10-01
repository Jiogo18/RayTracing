#include "fenetre.h"

#define PIXELS_WIDTH 150
#define PIXELS_HEIGHT 100

#ifndef GWL_USERDATA
#define GWL_USERDATA -21
#endif // GWL_USERDATA

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

    timerRefresh.setSingleShot(true);
    timerRefresh.connectOnFinished([this]() { this->refresh(); });

    // timerKeyPress.setInterval(40); // 25 FPS max (vitesse de déplacement & de rafraichissement auto)
    // QObject::connect(&timerKeyPress, &QTimer::timeout, this, &fenetre::updatePressPosition);
    // grabKeyboard();
}

fenetre::~fenetre() {}

void fenetre::keyPressEvent(int key, int status)
{
    if (status == 1077870593) return; // auto repeat == 403F0001 (KLF_RESET?)
    std::cout << "keyPressEvent: " << key << " " << status << std::endl;
    // if (KEY::keyConfig.contains(key)) {
    //     keysPressed |= (1 << KEY::keyConfig.value(key)); // add the key
    //     if (keysPressed && !timerKeyPress.isActive()) {
    //         timerKeyPress.start();
    //     }
    // } else {
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
    }
    // }
}

void fenetre::keyReleaseEvent(int key, int status)
{
    if (status == 1077870593) return; // auto repeat
    std::cout << "keyReleaseEvent: " << key << " " << status << std::endl;
    // if (KEY::keyConfig.contains(key)) {
    //     keysPressed ^= (1 << KEY::keyConfig.value(key)); // remove the key
    //     if (!keysPressed && timerKeyPress.isActive()) {
    //         timerKeyPress.stop();
    //     }
    // }
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

QSize fenetre::getRayTracingSize() const
{
    RECT rect;
    GetWindowRect(hWnd, &rect);
    return QSize(rect.right - rect.left - 16, rect.bottom - rect.top - 38);
}

fenetre *fenetre::getFenetreFromHWND(HWND hWnd)
{
    return reinterpret_cast<fenetre *>(GetWindowLongPtr(hWnd, GWL_USERDATA));
}

void fenetre::onWorkStarted()
{
    lastRefreshTime = DebugTime::getCurrent();
    // setCursor(Qt::WaitCursor);
}
void fenetre::onWorkFinished()
{
    // setCursor(Qt::ArrowCursor);
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
    if (keysPressed & (1 << KEY::keyAction::forward)) map->moveFront();
    if (keysPressed & (1 << KEY::keyAction::back)) map->moveBack();
    if (keysPressed & (1 << KEY::keyAction::left)) map->moveLeft();
    if (keysPressed & (1 << KEY::keyAction::right)) map->moveRight();
    if (keysPressed & (1 << KEY::keyAction::up)) map->moveUp();
    if (keysPressed & (1 << KEY::keyAction::down)) map->moveDown();
    if (keysPressed & (1 << KEY::keyAction::up_rot)) map->moveRZ(MouseSensibility / 30);
    if (keysPressed & (1 << KEY::keyAction::down_rot)) map->moveRZ(-MouseSensibility / 30);
    if (keysPressed & (1 << KEY::keyAction::left_rot)) map->moveRX(-MouseSensibility / 30);
    if (keysPressed & (1 << KEY::keyAction::right_rot)) map->moveRX(MouseSensibility / 30);

    //std::cout << "pos client " << map->getClient()->getPos() << std::endl;
    if (isPainting()) {
        // actu à la fin du repaint en cours
        timerRefresh.start(lastRefreshDuration);
    } else {
        refresh();
    }
}
