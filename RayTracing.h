#ifndef RAYTRACING_H
#define RAYTRACING_H

#include "3DObject/3DGeometry.h"
#include "3DObject/World3D.h"
#include <QVector>
#include <QPainter>
#include <QPointer>
#include <QThread>
#include "map3D.h"

#include "DebugTime.h"

#define REFRESH_COLUMN//TODO: remove it
//#define DISABLE_RAYPROCESS

namespace RAYTRACING {
    const doubli viewDistance = 100;
    const radian angleH = degreesToRadians(120);//de 156° à 172° max pour un Humain (1 oeil)
    const radian angleV = degreesToRadians(100);//180° = inf, 0° = 0
    const doubli xMax = tan(RAYTRACING::angleH / 2);
    const doubli yMax = tan(RAYTRACING::angleV / 2);
    const int pppH = 1;
    const int pppV = 1;
    const int ppp = pppV * pppH;
    const int WorkerThread = std::thread::hardware_concurrency() * 2;
    const int RefreshColumn = 200;
    const int gamma = 2;// TODO: réduire modifier ça lorsqu'on aura la lumière du soleil
    const int ColumnsPerWorker = 30;// il ne faut pas en attribuer trop sinon trop de colonnes pour un thread
}
using namespace RAYTRACING;



template<typename T>
class PixScreen
{
public://il faut au moins un PixScreen<T> dans ce fichier pour en avoir avec le même type dans d'autres fichiers
    PixScreen() { screen = QList<QList<T>>(0); };
    PixScreen(const QSize& size);
    PixScreen(const PixScreen& ps) { screen = ps.screen; };
    PixScreen* operator=(const PixScreen& ps) { screen = ps.screen; return this; };
    void set(const int& x, const int& y, T pos) { screen[x][y] = pos; };
    T at(const int& x, const int& y) const { return screen.at(x).at(y); };
    int width() const { return screen.length(); };
    int height() const { return screen.length() ? screen.at(0).length() : 0; };
    QSize size() const { return QSize(width(), height()); }
    int rowNumber() const { return width() * height(); };
    QList<T> getColumn(int x) const { return screen.at(x); };
    void setColumn(int x, QList<T> column) { screen[x] = column; };
private:
    QList<QList<T>> screen;
};


class RayTracingRessources
{
public:
    RayTracingRessources(const World* world, const Entity* client, DebugTime* dt);
    ~RayTracingRessources();
    const World* world = nullptr;
    DebugTime* dt = nullptr;
    Pos3D clientPos;
    QMap<QString, const QImage*>* facesImg = nullptr;
    void worldChanged();
    BLOCK::Material insideMaterial;
    void resetRessources(const Entity* client);
};



class Ray
{
public:
    Ray(Pos3D pos, RayTracingRessources* rtRess);
    ColorLight getColor() const;
    void process(const World* world);
private:
    Pos3D pos;
    QList<ColorLight> colors;
    const Face* lastFace = nullptr;
    bool lastMoved = false;
    int opacity = 0;
    //vector objTraverse;
    doubli distParcouru;
    void moveTo(const Point3D& newPos);
    const Face* getFirstIntersection(const World* world, Point3D& pInterMin) const;
    //black list (et white list pour transparence) du dernier objet traversé
    //par défaut: sortie du client (pour pas le retapper)
    //si rebond: dernier objet en mode "sortie"
    //si entrée (transparent): dernier objet en mode "entrée" puis "sortie" pour calculer l'épaisseur
    //si un objet dans l'objet entré est touché alors on sort du premier objet au point P et on rentre ds le 2
    //puis on resort du 2 on rerentre ds le 1 et resort du 1 (de l'autre coté de 2)
    bool enter = false;
    BLOCK::Material insideMaterial;
    RayTracingRessources* rtRess = nullptr;
    const QImage* getTexture(const QString& file) const;
};



class RayTracingWorker : public QThread
{
    Q_OBJECT
public:
    RayTracingWorker(int workerId, RayTracingRessources* rtRess, QObject* parent = nullptr);
    ~RayTracingWorker();
    RayTracingWorker* setPrimaryWork(const QSize& sceneSize);
    inline RayTracingWorker* setWork(int xScene) { this->xScene = xScene; return this; }
    inline int getWorkerId() const { return workerId; };
    inline int getNbColumns() const { return std::min(RAYTRACING::ColumnsPerWorker, sceneSize.width() - xScene); }

signals:
    void resultReady(int x, int nbColumns, const QList<ColorLight>* c, const int* totalLight);
private:
    static QList<Pos3D> calcRaysPosColumn(const doubli& xPos, const int& sceneHeight, const Pos3D& clientPos);
    void run() override;// process ONE column at a time
    int workerId;
    QSize sceneSize;

    int xScene;// just ONE column, more columns with RAYTRACING::ColumnsPerWorker
    RayTracingRessources* rtRess;
    int* totalLight = nullptr;
    QList<ColorLight>* colors = nullptr;
};


class RayTracing : public QThread
{
    Q_OBJECT
public:
    RayTracing(const map3D* map);
    ~RayTracing();

    RayTracing* setSize(const QSize& size) { if (!isRunning()) calcSize = size; return this; }
    const QImage& getImage() const { return image; }
    bool isRunning() { return QThread::isRunning() || workersInProcess; }

private slots:
    void worldChanged(const WorldChange& change);
    void onRayWorkerReady(int x, int nbColumns, const QList<ColorLight>* c, const int* totalLight);
    void onRayWorkerFinished();

signals:
    void resultReady(const QImage& image);

private:
    void run() override;
    void assignNextRayWork(RayTracingWorker* worker);
    double calcTotalLight() const;
    void paint();
    void onAllWorkersFinished();

    RayTracingRessources* rtRess = nullptr;
    DebugTime dt;
    qint64 startRun;

    const map3D* map;
    QSize calcSize;

    QList<doubli> yPosList; // calculs des yPos en fonction de y
    PixScreen<ColorLight> colors;
    QList<int> lightPerColumn; // calculs des light de chaque colonne (en fonction de x)
    int processWidth = 0;
    int processFinished = 0;
    int processStarted = 0;
    int processForUpdate;
    QList<RayTracingWorker*> rayWorkers;
    bool workersInProcess = false;
    QImage image;
};


/*
tests: equation du rayon:
z = sin(aH) * x => mais ça réduit si on augmente aV donc NON

y augmente? quand distance_origine augmente et que aV > 0
y = sin(aV) * distance_origine = sin(aV) * sqrt(x²+z²)
t temp en s
y = sin(aV)*t
x = cos(aH)*t*cos(aV)
z = sin(aH)*t*cos(aV)
<=> t = y/sin(aV) = x/cos(aH)cos(aV) = z/sin(aH)cos(aV)
<=> z/sin(aH) = x/cos(aH)
<=> z*cos(aH) = x*sin(aH)
<=> z = x*sin(aH)/cos(aH)
mais si aH = 90... ça marche plus
=> en 2D ça fait la même: y=a ou x=a (on peut pas mettre de x ou de y dans ces cas

un point en 1D? ax+b = 0
une droite en 2D? ax+by+c = 0
une plan en 3D? ax+by+cz+d = 0
il faut trouver l'équation du plan ou du volume du cube (?)
la droite il faut le point d'origine (client) et calculer un autre point => pour avoir le vecteur


equation d'un plan:
https://www.maxicours.com/se/cours/equation-cartesienne-d-un-plan/
https://www.youtube.com/watch?v=s4xqI6IPQBY
intersection d'un plan et d'une droite:
https://youtu.be/BYBMauyizhE

Pour commencer: #Orthogonal
verifier que AB et le plan P sont sécants:
=> si vecteur normal de P est orthogonal à AB alors on s'arrete
m(a, b, c) est un vecteur normal de P avec l'équation de P: ax+by+cz+d = 0
xm*xAB + ym*yAB + zm*zAB = 0 ? alors on s'arrete

#Intersection
Droite:
x = xA + xAB*t
y = yA + yAB*t
z = zA + zAB*t
Plan:
ax+by+cz+d = 0
<=> a*(xAB*t + xA) + b*(yAB*t + yA) + c*(zAB*t + zA) + d = 0
<=> (a*xAB + b*yAB + c*zAB)*t = -a*xA -b*yA -c*zA - d
<=> t = -(a*xA + b*yA + c*zA + d)/(a*xAB + b*yAB + c*zAB)


plus qu'a trouver a, b et c...
On connait 4 points du plan, on s'en sert que de 2
il faut donc trouver un vecteur normal n => perpendiculaire à 2 droites du plan => on prend AB et AC

n perpendiculaire AB et AC (et AB pas colinéaire à AC => pas le meme angle)
<=> n.AB = a*xAB + b*yAB + c*zAB = 0
<=> n.AC = a*xAC + b*yAC + c*zAC = 0


#EquationPlan
on a:	n.AB = xAB*a+yAB*b+zAB*c = 0
        n.AC = xAC*a+yAC*b+zAC*c = 0
1) on veut yAB*b - n*yAC*b = 0 (n Réel)
<=> nb = yAB/yAC
on a:
n.AB - n*n.AC = xAB*a + zAB*c - n(xAC*a + zAC*c) = 0
<=> c(zAB-zAC*n) = -(xAB-xAC*n)*a = (xAC*n-xAB)*a
<=> c = (xAC*n-xAB)*a/(zAB-zAC*n)

2)pareil avec zAB*c - n*zAC*c = 0 (n Réel)
<=> nc = zAB/zAC
<=> b = (xAC*n-xAB)*a/(yAB-yAC*n)

test avec -3a+b+2c = 0 et -7a-2b+4c = 0 (maxicours)
nb = yAB/yAC = 1/-2 = -1/2
c = (-7*-1/2 - -3)*a/(2 - 4*-1/2)
  = (7/2 + 3)*a/(2 + 2)
  = (13)/2*a/4
  = 13*a/8 OK
nc = zAB/zAC = 2/4 = 1/2
b = (-7*1/2 - -3)*a/(1 - -2*1/2)
  = (-7+6)/2 *a/(1+1)
  = -1*a/4 = -a/4 OK

après on détermine a comme on veut (1 par exemple)

on cherche d
on sait que A appartient au plan donc l'équation = 0
=> a*xA + b*yA + c*zA = -d

*/

#endif // RAYTRACING_H
