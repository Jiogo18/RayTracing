#include "../../3DObject/Pos3D.h"
#include "speeddebug.cpp"

void rotTest()
{
    debugVerifTime("Rot3D::Rot3D()", verifTime([] {
                       Rot3D a(1, 2);
                       return a;
                   }),
                   1990, 3010);

    debugVerifTime("Rot3D::Rot3D(1,2)", verifTime([] {
                       Rot3D a(1, 2);
                       return a;
                   }),
                   1990, 3100);

    doubli x = 1, y = 2;
    debugVerifTime("Rot3D::Rot3D(Rot3D)", verifTime_1([](const Rot3D &a) {
                       Rot3D b(a);
                   },
                                                      Rot3D(x, y)),
                   2000, 4010);

    debugVerifTime("Rot3D::fromVector", verifTime_1([](const Vec3D &v) {
                       Rot3D::fromVector(v);
                   },
                                                    Vec3D(1, 2, 3)),
                   89000, 91000);

    debugVerifTime("Rot3D::toVector", verifTime_1([](const Rot3D &r) {
                       r.toVector();
                   },
                                                  Rot3D(1, 2)),
                   221000, 227100);

    debugVerifTime("Rot3D::operator==", verifTime_2([](const Rot3D &a, const Rot3D &b) {
                       return a == b;
                   },
                                                    Rot3D(1, 2), Rot3D(1, 1)),
                   3990, 5000);

    qDebug();
}

void posTestConstructor()
{
    debugVerifTime("Pos3D()", verifTime([] {
                       Pos3D p; // 48 ms
                       Q_UNUSED(p)
                   }),
                   4900, 5505);

    debugVerifTime("Pos3D(0,0,0)", verifTime([] {
                       Pos3D p(0, 0, 0, 0, 0); // 300 ms
                       Q_UNUSED(p)
                   }),
                   4900, 6100);

    debugVerifTime("Pos3D(1,2,3)", verifTime([] {
                       Pos3D p(1, 2, 3, 4, 5); // 300 ms
                       Q_UNUSED(p)
                   }),
                   6900, 7210);

    debugVerifTime("Pos3D(x,y,z)", verifTime([] {
                       doubli x = 1, y = 2, z = 3, rx = 4, rz = 5;
                       Pos3D p(x, y, z, rx, rz); // 244 ms
                   }),
                   34900, 36100);

    Pos3D p(1, 2, 3, 4, 5);

    debugVerifTime("Pos3D::setPoint()", verifTime_2([](const Pos3D &p, Pos3D a) {
                       a.setPoint(p);
                   },
                                                    p, Pos3D()),
                   10500, 12000);

    debugVerifTime("Pos3D::operator=", verifTime_2([](const Pos3D &p, Pos3D a) {
                       a = p;
                   },
                                                   p, Pos3D()),
                   5000, 6100);

    debugVerifTime("Pos3D::copy", verifTime_1([](const Pos3D &p) {
                       Pos3D a(p);
                       Q_UNUSED(a)
                   },
                                              p),
                   2900, 4100);

    qDebug();
}

void posTestBasic()
{
    Pos3D p(1, 2, 3, 4, 5);
    debugVerifTime("Pos3D::getX()", verifTime_1([](const Pos3D &p) {
                       p.x();
                   },
                                                p),
                   3000, 3310);

    debugVerifTime("Pos3D::getPoint()", verifTime_1([](const Pos3D &p) {
                       p.getPoint();
                   },
                                                    p),
                   3000, 4100);

    debugVerifTime("Pos3D::setX()", verifTime_1([](Pos3D p) {
                       p.setX(10);
                   },
                                                p),
                   7980, 9010);

    debugVerifTime("Pos3D::addX()", verifTime_1([](Pos3D p) {
                       p.addX(10);
                   },
                                                p),
                   8000, 9100);

    qDebug();
}

void posTestComparator()
{
    Pos3D p(1, 2, 3, 4, 5), p2(4, 0, -6, 0, 3);

    debugVerifTime("Pos3D::operator<", verifTime_2([](const Pos3D &a, const Pos3D &b) {
                       return a < b;
                   },
                                                   p, p2),
                   6980, 7600);

    debugVerifTime("Pos3D::operator>", verifTime_2([](const Pos3D &a, const Pos3D &b) {
                       return a > b;
                   },
                                                   p, p2),
                   6980, 7200);

    debugVerifTime("Pos3D::operator<=", verifTime_2([](const Pos3D &a, const Pos3D &b) {
                       return a <= b;
                   },
                                                    p, p2),
                   6980, 7110);

    debugVerifTime("Pos3D::operator>=", verifTime_2([](const Pos3D &a, const Pos3D &b) {
                       return a >= b;
                   },
                                                    p, p2),
                   6980, 8010);

    debugVerifTime("Pos3D::operator==", verifTime_2([](const Pos3D &a, const Pos3D &b) {
                       return a == b;
                   },
                                                    p, p2),
                   4700, 5110);

    debugVerifTime("Pos3D::operator!=", verifTime_2([](const Pos3D &a, const Pos3D &b) {
                       return a != b;
                   },
                                                    p, p2),
                   4980, 5100);

    debugVerifTime("Pos3D::isNull()", verifTime_1([](const Pos3D &a) {
                       return a.isNull();
                   },
                                                  p),
                   3700, 4400);

    debugVerifTime("Pos3D::isInf()", verifTime_1([](const Pos3D &a) {
                       return a.isInf();
                   },
                                                 p),
                   7000, 8100);

    debugVerifTime("Pos3D::isNan()", verifTime_1([](const Pos3D &a) {
                       a.isNan();
                   },
                                                 p),
                   5980, 6110);

    qDebug();
}

void posTestOperator()
{
    Pos3D p(1, 2, 3, 4, 5), p2(4, 0, -6, 0, 3);
    doubli k = 2.1;

    debugVerifTime("Pos3D::operator+", verifTime_2([](const Pos3D &a, const Pos3D &b) {
                       a + b;
                   },
                                                   p, p2),
                   36900, 38100);

    debugVerifTime("Pos3D::operator-", verifTime_2([](const Pos3D &a, const Pos3D &b) {
                       a - b;
                   },
                                                   p, p2),
                   37000, 38100);

    debugVerifTime("Pos3D::operator*", verifTime_2([](const Pos3D &a, const Pos3D &b) {
                       a *b;
                   },
                                                   p, p2),
                   37000, 38100);

    debugVerifTime("Pos3D::operator*k", verifTime_2([](const Pos3D &a, const doubli &k) {
                       a *k;
                   },
                                                    p, k),
                   37000, 42100);

    debugVerifTime("Pos3D::operator/", verifTime_2([](const Pos3D &a, const Pos3D &b) {
                       a / b;
                   },
                                                   p, p2),
                   37000, 41200);

    debugVerifTime("Pos3D::operator/k", verifTime_2([](const Pos3D &a, const doubli &k) {
                       a / k;
                   },
                                                    p, k),
                   36400, 39100);

    debugVerifTime("Pos3D::unary-", verifTime_1([](const Pos3D &a) {
                       return -a;
                   },
                                                p),
                   36000, 37010);

    qDebug();
}

void posTestDistance()
{
    Pos3D p(1, 2, 3, 4, 5), p2(4, 0, -6, 0, 3);

    debugVerifTime("Pos3D::distanceAxeZ", verifTime_1([](const Pos3D &a) {
                       a.distanceAxeZ();
                   },
                                                      p),
                   13000, 17100);

    debugVerifTime("Pos3D::distanceOrigine", verifTime_1([](const Pos3D &a) {
                       a.distanceOrigine();
                   },
                                                         p),
                   16000, 16100);

    debugVerifTime("Pos3D::distance", verifTime_2([](const Pos3D &a, const Pos3D &b) {
                       a.distance(b);
                   },
                                                  p, p2),
                   21900, 23000);

    debugVerifTime("Pos3D::distance2", verifTime_2([](const Pos3D &a, const Pos3D &b) {
                       Pos3D::distance(a, b);
                   },
                                                   p, p2),
                   21900, 22100);

    debugVerifTime("Pos3D::distanceMax", verifTime_2([](const Pos3D &a, const Pos3D &b) {
                       a.distanceMax(b);
                   },
                                                     p, p2),
                   25000, 26100);

    debugVerifTime("Pos3D::distanceMax2", verifTime_2([](const Pos3D &a, const Pos3D &b) {
                       Pos3D::distanceMax(a, b);
                   },
                                                      p, p2),
                   25000, 26100);

    qDebug();
}

void posTest()
{
    posTestConstructor();
    posTestBasic();
    posTestComparator();
    posTestOperator();
    posTestDistance();

    qDebug();
}
