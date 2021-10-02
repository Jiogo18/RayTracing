
#include "GUI/fenetre.h"

bool CreateConsole()
{
    // https://stackoverflow.com/a/57241985/12908345
    if (!AttachConsole(ATTACH_PARENT_PROCESS) && !AllocConsole()) {
        return false;
    }

    FILE *fDummy;
    freopen_s(&fDummy, "CONOUT$", "w", stdout);
    freopen_s(&fDummy, "CONOUT$", "w", stderr);
    freopen_s(&fDummy, "CONIN$", "r", stdin);
    std::cout.clear();
    std::clog.clear();
    std::cerr.clear();
    std::cin.clear();

    return true;
}

class Application
{
public:
    Application(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
    {
        map = new map3D();
        window = new fenetre(hInstance, map);
        window->show(nShowCmd);
    }
    ~Application()
    {
        if (window != nullptr) delete window;
        if (map != nullptr) delete map;
    }
    int exec()
    {
        return window->exec();
    }

private:
    map3D *map = nullptr;
    fenetre *window;
};

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
#ifdef _WINDOWS
    if (!CreateConsole()) return -1;
#endif
    std::cout << "[WinMain] starting..." << std::endl;
    Application app(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
    std::cout << "[WinMain] Window created!" << std::endl;
    return app.exec();
}
