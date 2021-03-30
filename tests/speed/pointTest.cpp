#include "../../3DObject/Point3D.h"
#include "speeddebug.cpp"

void pointTestConstructor()
{
    debugVerifTime("Point3D()", verifTime([] {
                       Point3D p; // 48 ms
                       Q_UNUSED(p)
                   }),
                   4900, 5505);

    debugVerifTime("Point3D(0,0,0)", verifTime([] {
                       Point3D p(0, 0, 0); // 300 ms
                       Q_UNUSED(p)
                   }),
                   4900, 6100);

    debugVerifTime("Point3D(1,2,3)", verifTime([] {
                       Point3D p(1, 2, 3); // 300 ms
                       Q_UNUSED(p)
                   }),
                   6900, 7210);

    debugVerifTime("Point3D(x,y,z)", verifTime([] {
                       doubli x = 1, y = 2, z = 3;
                       Point3D p(x, y, z); // 244 ms
                   }),
                   34900, 36100);

    Point3D p(1, 2, 3);

    debugVerifTime("Point3D::setPoint()", verifTime_2([](const Point3D &p, Point3D a) {
                       a.setPoint(p);
                   },
                                                      p, Point3D()),
                   10500, 12000);

    debugVerifTime("Point3D::operator=", verifTime_2([](const Point3D &p, Point3D a) {
                       a = p;
                   },
                                                     p, Point3D()),
                   5000, 6100);

    debugVerifTime("Point3D::copy", verifTime_1([](const Point3D &p) {
                       Point3D a(p);
                       Q_UNUSED(a)
                   },
                                                p),
                   2900, 4100);
    qDebug();
}

void pointTestBasic()
{
    Point3D p(1, 2, 3);
    debugVerifTime("Point3D::getX()", verifTime_1([](const Point3D &p) {
                       p.x();
                   },
                                                  p),
                   3000, 3310);

    debugVerifTime("Point3D::getPoint()", verifTime_1([](const Point3D &p) {
                       p.getPoint();
                   },
                                                      p),
                   3000, 4100);

    debugVerifTime("Point3D::setX()", verifTime_1([](Point3D p) {
                       p.setX(10);
                   },
                                                  p),
                   7980, 9010);

    debugVerifTime("Point3D::addX()", verifTime_1([](Point3D p) {
                       p.addX(10);
                   },
                                                  p),
                   8000, 9100);
    qDebug();
}

void pointTestComparator()
{
    Point3D p(1, 2, 3), p2(4, 0, -6);

    debugVerifTime("Point3D::operator<", verifTime_2([](const Point3D &a, const Point3D &b) {
                       return a < b;
                   },
                                                     p, p2),
                   6980, 7600);

    debugVerifTime("Point3D::operator>", verifTime_2([](const Point3D &a, const Point3D &b) {
                       return a > b;
                   },
                                                     p, p2),
                   6980, 7200);

    debugVerifTime("Point3D::operator<=", verifTime_2([](const Point3D &a, const Point3D &b) {
                       return a <= b;
                   },
                                                      p, p2),
                   6980, 7110);

    debugVerifTime("Point3D::operator>=", verifTime_2([](const Point3D &a, const Point3D &b) {
                       return a >= b;
                   },
                                                      p, p2),
                   6980, 8010);

    debugVerifTime("Point3D::operator==", verifTime_2([](const Point3D &a, const Point3D &b) {
                       return a == b;
                   },
                                                      p, p2),
                   4700, 5110);

    debugVerifTime("Point3D::operator!=", verifTime_2([](const Point3D &a, const Point3D &b) {
                       return a != b;
                   },
                                                      p, p2),
                   4980, 5100);

    debugVerifTime("Point3D::isNull()", verifTime_1([](const Point3D &a) {
                       return a.isNull();
                   },
                                                    p),
                   3700, 4400);

    debugVerifTime("Point3D::isInf()", verifTime_1([](const Point3D &a) {
                       return a.isInf();
                   },
                                                   p),
                   7000, 8100);

    debugVerifTime("Point3D::isNan()", verifTime_1([](const Point3D &a) {
                       a.isNan();
                   },
                                                   p),
                   5980, 6110);
    qDebug();
}

void pointTestOperator()
{
    Point3D p(1, 2, 3), p2(4, 0, -6);
    doubli k = 2.1;

    debugVerifTime("Point3D::operator+", verifTime_2([](const Point3D &a, const Point3D &b) {
                       a + b;
                   },
                                                     p, p2),
                   36900, 38100);

    debugVerifTime("Point3D::operator-", verifTime_2([](const Point3D &a, const Point3D &b) {
                       a - b;
                   },
                                                     p, p2),
                   37000, 38100);

    debugVerifTime("Point3D::operator*", verifTime_2([](const Point3D &a, const Point3D &b) {
                       a *b;
                   },
                                                     p, p2),
                   37000, 38100);

    debugVerifTime("Point3D::operator*k", verifTime_2([](const Point3D &a, const doubli &k) {
                       a *k;
                   },
                                                      p, k),
                   37000, 42100);

    debugVerifTime("Point3D::operator/", verifTime_2([](const Point3D &a, const Point3D &b) {
                       a / b;
                   },
                                                     p, p2),
                   37000, 41200);

    debugVerifTime("Point3D::operator/k", verifTime_2([](const Point3D &a, const doubli &k) {
                       a / k;
                   },
                                                      p, k),
                   36400, 39100);

    debugVerifTime("Point3D::unary-", verifTime_1([](const Point3D &a) {
                       return -a;
                   },
                                                  p),
                   36000, 37010);
    qDebug();
}

void pointTestDistance()
{
    Point3D p(1, 2, 3), p2(4, 0, -6);

    debugVerifTime("Point3D::distanceAxeZ", verifTime_1([](const Point3D &a) {
                       a.distanceAxeZ();
                   },
                                                        p),
                   13000, 17100);

    debugVerifTime("Point3D::distanceOrigine", verifTime_1([](const Point3D &a) {
                       a.distanceOrigine();
                   },
                                                           p),
                   16000, 16100);

    debugVerifTime("Point3D::distance", verifTime_2([](const Point3D &a, const Point3D &b) {
                       a.distance(b);
                   },
                                                    p, p2),
                   21900, 23000);

    debugVerifTime("Point3D::distance2", verifTime_2([](const Point3D &a, const Point3D &b) {
                       Point3D::distance(a, b);
                   },
                                                     p, p2),
                   21900, 22100);

    debugVerifTime("Point3D::distanceMax", verifTime_2([](const Point3D &a, const Point3D &b) {
                       a.distanceMax(b);
                   },
                                                       p, p2),
                   25000, 26100);

    debugVerifTime("Point3D::distanceMax2", verifTime_2([](const Point3D &a, const Point3D &b) {
                       Point3D::distanceMax(a, b);
                   },
                                                        p, p2),
                   25000, 26100);
    qDebug();
}

void pointTest()
{
    pointTestConstructor();
    pointTestBasic();
    pointTestComparator();
    pointTestOperator();
    pointTestDistance();

    qDebug();
}

void vecTest()
{
    debugVerifTime("Vec3D::Vec3D(1,2,3)", verifTime([] {
                       //const doubli x = 1, y = 2, z = 3;
                       //Vec3D a(x, y, z);
                       Vec3D a(1, 2, 3);
                   }),
                   7000, 8100);
    //}), 19000, 21000);

    Vec3D a(1, 2, 3), b(4, 5, -6);
    debugVerifTime("Vec3D::produitScalaire", verifTime_2([](const Vec3D &a, const Vec3D &b) {
                       a.produitScalaire(b);
                   },
                                                         a, b),
                   9000, 10100);

    debugVerifTime("Vec3D::unitary-", verifTime_1([](const Vec3D &a) {
                       return -a;
                   },
                                                  a),
                   21000, 22100);

    qDebug();
}
