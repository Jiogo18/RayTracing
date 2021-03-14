#include "fenetre.h"

fenetre::fenetre(QWidget *parent) : QOpenGLWidget(parent)
{
    map = new map3D();

    gui = new GUI(map, this);

    connect(gui, &GUI::workStarted, this, &fenetre::workStarted);
    connect(gui, &GUI::workReady, this, &fenetre::workFinished);
//    connect(map, &map3D::PBMax, this, &fenetre::setPBMax);
//    connect(map, &map3D::PBValue, this, &fenetre::setPBValue);

    //setCursor(Qt::BlankCursor);

    //winTaskbarButt = new QWinTaskbarButton(this);

    show();
    setMinimumSize(50,50);
    setGeometry(QRect(geometry().topLeft(), QSize(150, 100)));

    //moveMouseMidWindow();
    //posMouse = QPoint(0, 0);


    lastRefreshTime = 0;
    actualise();

    timerRefresh.setSingleShot(true);
    QObject::connect(&timerRefresh, &QTimer::timeout, this, &fenetre::actualise);
}

fenetre::~fenetre()
{
    if(gui != nullptr) delete gui;
    if(map != nullptr) delete map;
    //if(button != nullptr) delete button;
}

void fenetre::actualise()
{
    lastRefreshTime = QDateTime::currentMSecsSinceEpoch();
    gui->refresh();
}


void fenetre::keyPressEvent(QKeyEvent *event)
{
    const Pos3D &previousPos = map->getClient()->getPos();
    switch (event->key()) {
    case Qt::Key_Z:
        map->moveFront();
        break;
    case Qt::Key_Q:
        map->moveLeft();
        break;
    case Qt::Key_S:
        map->moveBack();
        break;
    case Qt::Key_D:
        map->moveRight();
        break;
    case Qt::Key_Shift:
        map->moveDown();
        break;
    case Qt::Key_Space:
        map->moveUp();
        break;
    case Qt::Key_Left:
        map->moveRX(-MouseSensibility/10);
        break;
    case Qt::Key_Right:
        map->moveRX(MouseSensibility/10);
        break;
    case Qt::Key_Up:
        map->moveRZ(MouseSensibility/10);
        break;
    case Qt::Key_Down:
        map->moveRZ(-MouseSensibility/10);
        break;
    case Qt::Key_F5:
        actualise();
        break;
    }
    qDebug() << "pos client" << map->getClient()->getPos();
    if(!(map->getClient()->getPos() == previousPos) && lastRefreshDuration < 300) {
        //si on a bougé et que ça prend pas trop de temps
        actualise();
        if(lastRefreshTime + 200 < QDateTime::currentMSecsSinceEpoch() ) {
            //si ça fait trop longtemps qu'on a pas update
            timerRefresh.stop();
            actualise();
        }
        else {
            //actu dans 40 msec ou plus si on reviens ici
            timerRefresh.start(lastRefreshDuration / 4);
        }
    }
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

void fenetre::workStarted()
{
    setCursor(Qt::WaitCursor);
    //button->progress()->setVisible(true);
}
void fenetre::workFinished() {
    setCursor(Qt::ArrowCursor);
    //button->progress()->setVisible(false);
    lastRefreshDuration = QDateTime::currentMSecsSinceEpoch() - lastRefreshTime;
    repaint();
}

//void fenetre::setPBMax(int max) { /*button->progress()->setMaximum(max);*/ }
//void fenetre::setPBValue(int value) { /*button->progress()->setValue(value);*/ }

void fenetre::paintEvent(QPaintEvent *event)
{
    // see 2D Painting Example (with OpenGL)
    Q_UNUSED(event);
    QPainter painter;
    painter.begin(this);
    painter.setRenderHint(QPainter::Antialiasing);
    gui->paint(&painter, event);
    painter.end();
}
