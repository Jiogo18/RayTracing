#include "GUI.h"

GUI::GUI(const map3D *map) : workerThread(new RayTracing(map)), map(map), rayImage(workerThread->getImage())
{
    workerThread->connectResultReady([this]() { this->handleWorkerResults(); });
    previousFPS = 0;
    frameCounter = 0;
}

GUI::~GUI()
{
    delete[] hBmpPixels;
    hBmpPixels = nullptr;
    DeleteObject(hBitmap);

    workerThread->quit();
    workerThread->wait(5000);
    delete workerThread;
}

// actualiser toute l'image (pour le GUIWorker)
void GUI::refresh()
{
    if (isPainting()) return;

    workStartedCallback();

    workerThread->setSize(getRayTracingSize())->start();
}

void GUI::switchFPSCounterVisible()
{
    showFPSCounter = !showFPSCounter;
}

void GUI::updateFPSCounter()
{
    previousFPS = frameCounter;
    frameCounter = 0;
}

void GUI::connectOnWorkStarted(std::function<void()> callback)
{
    workStartedCallback = callback;
}
void GUI::connectOnWorkFinished(std::function<void()> callback)
{
    workFinishedCallback = callback;
}

// garder l'image d'origine mais changer la taille de l'image à l'écran
void GUI::paintEvent(HWND hWnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);

    QSize size = getWindowSize();
    if (size.width() != MyBMInfo.bmiHeader.biWidth || size.height() != MyBMInfo.bmiHeader.biHeight) {
        DeleteObject(hBitmap);
        hBitmap = NULL;
        delete[] hBmpPixels;
        hBmpPixels = nullptr;
    }
    if (!hBmpPixels) {
        hBitmap = CreateCompatibleBitmap(hdc, size.width(), size.height());

        // get the bitmap header
        MyBMInfo.bmiHeader.biSize = sizeof(MyBMInfo.bmiHeader); // LPBITMAPINFO lpbmi
        GetDIBits(hdc, hBitmap, 0, 0, NULL, &MyBMInfo, DIB_RGB_COLORS);

        // create the bitmap buffer
        hBmpPixels = new BYTE[MyBMInfo.bmiHeader.biSizeImage];
        // Better do this here - the original bitmap might have BI_BITFILEDS, which makes it
        // necessary to read the color table - you might not want this.
        MyBMInfo.bmiHeader.biCompression = BI_RGB;

        // get the actual bitmap buffer
        GetDIBits(hdc, hBitmap, 0, MyBMInfo.bmiHeader.biHeight, hBmpPixels, &MyBMInfo, DIB_RGB_COLORS);
    }

    Image image = rayImage->toImage();
    image.fillBitmapPixels(hBmpPixels, MyBMInfo.bmiHeader.biSizeImage);
    SetDIBits(hdc, hBitmap, 0, MyBMInfo.bmiHeader.biHeight, hBmpPixels, &MyBMInfo, DIB_RGB_COLORS);

    HBRUSH brush = CreatePatternBrush(hBitmap);
    FillRect(hdc, &ps.rcPaint, brush);
    DeleteObject(brush);

    if (showFPSCounter) {
        SetTextColor(hdc, RGB(255, 255, 255));
        SetBkMode(hdc, TRANSPARENT);
        SetBkColor(hdc, 0);
        std::string textFPS = std::to_string(previousFPS);
        RECT textFPSRect = {0, 0, 30, 20};

        const TCHAR *pstring = nullptr;
#ifdef UNICODE
        std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
        std::wstring wstr = converter.from_bytes(textFPS);
        pstring = wstr.data();
#else
        pstring = textFPS.data();
#endif
        DrawText(hdc, pstring, textFPS.length(), &textFPSRect, DT_LEFT);
    }

    EndPaint(hWnd, &ps);
    ReleaseDC(hWnd, hdc);
}

// toute l'image a été actualisée (par le GUIWorker)
void GUI::handleWorkerResults()
{
    frameCounter++;
    // repaint();
    if (!isPainting()) {
        workFinishedCallback();
    }
}

QSize GUI::getRayTracingSize() const
{
    return getWindowSize();
}
