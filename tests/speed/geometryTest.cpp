#include "../../src/3D/Geometry3D.h"
#include "speeddebug.cpp"

void hrectTest()
{

    HRect3D p(Point3D(1, 2, 3), Point3D(2, 4, 6));
    debugVerifTime("HRect3D::containsLine()", verifTime_1([](const HRect3D &p) {
                       p.containsLine(Point3D(3, 5, 2), Point3D(-1, -2, 1), 5);
                   },
                                                          p),
                   213000, 221700);

    qDebug();
}

void geometryTest()
{
    hrectTest();
    qDebug();
}
