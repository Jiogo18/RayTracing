#include "fenetre.h"

fenetre::fenetre(map3D* map, QWidget* parent) : GUI(map, parent)
{
    this->map = map;

    connect(this, &GUI::workStarted, this, &fenetre::workStarted);
    connect(this, &GUI::workFinished, this, &fenetre::workFinished);
    //    connect(map, &map3D::PBMax, this, &fenetre::setPBMax);
    //    connect(map, &map3D::PBValue, this, &fenetre::setPBValue);

        //setCursor(Qt::BlankCursor);

        //winTaskbarButt = new QWinTaskbarButton(this);

    show();
    setMinimumSize(50, 50);
    setGeometry(QRect(geometry().topLeft(), QSize(150, 100)));

    //moveMouseMidWindow();
    //posMouse = QPoint(0, 0);


    lastRefreshTime = 0;
    refresh();

    timerRefresh.setSingleShot(true);
    QObject::connect(&timerRefresh, &QTimer::timeout, this, &GUI::refresh);
}

fenetre::~fenetre()
{
    //if(button != nullptr) delete button;
}


void fenetre::keyPressEvent(QKeyEvent* event)
{
    const Pos3D& previousPos = map->getClient()->getPos();
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
        map->moveRX(-MouseSensibility / 10);
        break;
    case Qt::Key_Right:
        map->moveRX(MouseSensibility / 10);
        break;
    case Qt::Key_Up:
        map->moveRZ(MouseSensibility / 10);
        break;
    case Qt::Key_Down:
        map->moveRZ(-MouseSensibility / 10);
        break;
    case Qt::Key_F5:
        refresh();
        break;
    }
    qDebug() << "pos client" << map->getClient()->getPos();
    if (map->getClient()->getPos() != previousPos && lastRefreshDuration < 300) {
        //si on a bougé et que ça prend pas trop de temps
        if (isPainting()) {
            //actu à la fin du repaint en cours
            timerRefresh.start(lastRefreshDuration);
        }
        else {
            refresh();
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
    lastRefreshTime = QDateTime::currentMSecsSinceEpoch();
    setCursor(Qt::WaitCursor);
    //button->progress()->setVisible(true);
}
void fenetre::workFinished() {
    setCursor(Qt::ArrowCursor);
    //button->progress()->setVisible(false);
    lastRefreshDuration = QDateTime::currentMSecsSinceEpoch() - lastRefreshTime;
}

//void fenetre::setPBMax(int max) { /*button->progress()->setMaximum(max);*/ }
//void fenetre::setPBValue(int value) { /*button->progress()->setValue(value);*/ }



