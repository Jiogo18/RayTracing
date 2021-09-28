#pragma once

#include "QSize.h"
#include "QColor.h"

class QImage
{
public:
    enum Format {
        Format_RGB32,
        Format_RGBA64
    };
    QImage() {}
    QImage(int width, int height, QImage::Format format) : widthp(width), heightp(height) {}
    QImage(const QSize &size) : widthp(size.width()), heightp(size.height()) {}
    QImage(const uchar *data, int width, int height, QImage::Format format) : widthp(width), heightp(height) {}

    int width() const { return widthp; }
    int height() const { return heightp; }

    bool isNull() const { return true; }
    QImage scaled(const QSize &size) const { return QImage(size); }
    QImage convertToFormat(QImage::Format) { return *this; }
    QImage::Format format() const { return Format::Format_RGBA64; }
    bool load(std::string filename) { return false; }

    QColor pixelColor(int x, int y) const { return QColor(0, 0, 0); }

    void fill(QColor color) {}
    void setPixelColor(int x, int y, QColor color) {}

private:
    int widthp;
    int heightp;
};