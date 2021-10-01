#include "Point2D.h"

Point2D Point2D::operator-(const Point2D &p) const
{
    return Point2D(xp - p.xp, yp - p.yp);
}

std::ostream &operator<<(std::ostream &os, const Point2D &p)
{
    os << "Point2D(" << p.xp << "," << p.yp << ")";
    return os;
}