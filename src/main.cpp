
#include "GUI/fenetre.h"

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
    std::cout << "[WinMain] starting..." << std::endl;
    Application app(hInstance, hPrevInstance, lpCmdLine, nShowCmd);
    std::cout << "[WinMain] Window created!" << std::endl;
    return app.exec();
}
