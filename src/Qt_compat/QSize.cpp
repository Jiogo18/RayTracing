#include "QSize.h"

std::ostream &operator<<(std::ostream &os, const QSize &size)
{
    os << "QSize(" << size.widthp << "," << size.heightp << ")";
    return os;
}