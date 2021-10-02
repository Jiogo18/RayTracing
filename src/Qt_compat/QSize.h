#pragma once

#include <iostream>
#include "../3D/maths3D.h"

class QSize
{
public:
    constexpr inline QSize() : widthp(0), heightp(0) {}
    constexpr inline QSize(int width, int height) : widthp(width), heightp(height) {}

    constexpr inline int width() const { return widthp; }
    int height() const { return heightp; }

    constexpr inline bool operator==(const QSize &other) const { return widthp == other.widthp && heightp == other.heightp; }
    constexpr inline bool operator!=(const QSize &other) const { return widthp != other.widthp || heightp != other.heightp; }

    friend std::ostream &operator<<(std::ostream &os, const QSize &size);

private:
    int widthp;
    int heightp;
};
