#pragma once

#include "QSize.h"
#include "QColor.h"
#include <windows.h>
#include <wingdi.h>

typedef unsigned char uchar;

class Image
{
public:
    enum Format {
        Format_RGB32,
        Format_RGBA64
    };
    Image();
    Image(int width, int height, Image::Format format = Format::Format_RGB32);
    Image(const QSize &size);
    Image(const uchar *data, int width, int height, Image::Format format);
    Image(const Image &image);
    ~Image();

    Image &operator=(const Image &image);

    int width() const { return widthp; }
    int height() const { return heightp; }

    bool isNull() const { return widthp == 0 || heightp == 0; }
    Image scaled(const QSize &size) const;
    Image convertToFormat(Image::Format) { return *this; }
    Image::Format format() const { return Format::Format_RGBA64; }
    bool load(std::string filename) { return false; }

    QColor pixelColor(int x, int y) const;
    COLORREF pixelRGB(int x, int y) const;
    void fillBitmapPixels(BYTE *bytes, int size) const;

    void fill(QColor color);
    void setPixelColor(int x, int y, QColor color);

private:
    int widthp;
    int heightp;
    uchar *red = nullptr;
    uchar *green = nullptr;
    uchar *blue = nullptr;
    int getPixels() const { return widthp * heightp; }
};