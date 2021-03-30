#include "fenetre.h"

fenetre::fenetre(map3D *map, QWidget *parent) : GUI(map, parent)
{
    this->map = map;

    connect(this, &GUI::workStarted, this, &fenetre::onWorkStarted);
    connect(this, &GUI::workFinished, this, &fenetre::onWorkFinished);
    // setCursor(Qt::BlankCursor);

    show();
    setMinimumSize(50, 50);
    setGeometry(QRect(geometry().topLeft(), QSize(150, 100)));

    // moveMouseMidWindow();
    // posMouse = QPoint(0, 0);

    lastRefreshTime = 0;
    refresh();

    timerRefresh.setSingleShot(true);
    QObject::connect(&timerRefresh, &QTimer::timeout, this, &GUI::refresh);

    timerKeyPress.setInterval(40);
    QObject::connect(&timerKeyPress, &QTimer::timeout, this, &fenetre::updatePressPosition);
    grabKeyboard();
}

fenetre::~fenetre() {}

void fenetre::keyPressEvent(QKeyEvent *event)
{
    if (KEY::keyConfig.contains(event->key())) {
        if (event->isAutoRepeat()) return;
        keysPressed |= (1 << KEY::keyConfig.value(event->key())); // add the key
        if (keysPressed && !timerKeyPress.isActive()) {
            timerKeyPress.start();
        }
    } else {
        switch (event->key()) {
        case Qt::Key_F5:
            refresh();
            break;
        case Qt::Key_F7:
            speedTest();
            break;
        }
    }
}

void fenetre::keyReleaseEvent(QKeyEvent *event)
{
    if (KEY::keyConfig.contains(event->key())) {
        if (event->isAutoRepeat()) return;
        keysPressed ^= (1 << KEY::keyConfig.value(event->key())); // remove the key
        if (!keysPressed && timerKeyPress.isActive()) {
            timerKeyPress.stop();
        }
    }
}

void fenetre::speedTest()
{
    if (testSpeedActivated) return;
    testSpeedActivated = true;
    testSpeedCounter = 0;
    testSpeedTime = QDateTime::currentMSecsSinceEpoch();
    qDebug() << "speedTest started for 1000 msec";
    refresh();
}

// void fenetre::mouseMoveEvent(QMouseEvent *event)
//{
//    Q_UNUSED(event)
//    //map->getClient()->moveRX((MidWindow().x() - event->x())/MouseSensibility);
//    //map->getClient()->moveRY((MidWindow().y() - event->y())/MouseSensibility);
//    //moveMouseMidWindow();
//}

// QPoint fenetre::MidWindow() { return QPoint(width()/2, height()/2); }
// void fenetre::moveMouseMidWindow() { QCursor::setPos(MidWindow() + QWidget::pos()); }

void fenetre::onWorkStarted()
{
    lastRefreshTime = QDateTime::currentMSecsSinceEpoch();
    setCursor(Qt::WaitCursor);
}
void fenetre::onWorkFinished()
{
    setCursor(Qt::ArrowCursor);
    lastRefreshDuration = QDateTime::currentMSecsSinceEpoch() - lastRefreshTime;
    if (testSpeedActivated) onSpeedTestFinished();
}

void fenetre::onSpeedTestFinished()
{
    testSpeedCounter++;
    qint64 duration = QDateTime::currentMSecsSinceEpoch() - testSpeedTime;
    if (duration >= 2000) {
        // end
        qDebug() << "speedTest with" << testSpeedCounter << "refresh in" << duration << "msec ("
                 << (duration / testSpeedCounter) << "msec/frame,"
                 << ((float)testSpeedCounter / duration * 1000) << "FPS )";
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

    //    qDebug() << "pos client" << map->getClient()->getPos();
    if (isPainting()) {
        // actu à la fin du repaint en cours
        timerRefresh.start(lastRefreshDuration);
    } else {
        refresh();
    }
}
