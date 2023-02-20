
#include "GUI/fenetre.h"
#include "RayTracing/GPUCalls.h"

class Application
{
public:
    Application()
    {
        map = new map3D();
        window = new fenetre(map);
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

int main(int argc, char *argv[])
{
    glutInit(&argc, argv); // initialise GLUT
    GPUCalls::init();
    std::cout << "[WinMain] starting..." << std::endl;
    Application app;
    std::cout << "[WinMain] Window created!" << std::endl;
    return app.exec();
}
