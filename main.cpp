#include "fenetre.h"
#include <QApplication>


class Application : public QApplication
{
public:
    Application(int& argc, char** argv) : QApplication(argc, argv)
    {
        map = new map3D();
        gui = new fenetre(map);
    }
    ~Application() override
    {
        if (gui != nullptr) delete gui;
        if (map != nullptr) delete map;
    }

private:
    map3D* map = nullptr;
    fenetre* gui = nullptr;
};



int main(int argc, char* argv[]) {
    Application app(argc, argv);

    return app.exec();
}
