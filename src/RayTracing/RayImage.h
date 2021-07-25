#ifndef RAYIMAGE_H
#define RAYIMAGE_H

#include "ColorLight.h"
#include <QSize>
#include <QImage>
#include <QPixmap>

class RayImage
{
public:
    RayImage();
    RayImage(int width, int height);
    RayImage(const QSize &size);
    RayImage(const RayImage &that);
    ~RayImage();

    RayImage &operator=(const RayImage &that);
    RayImage scaled(const QSize &size) const;
    QImage toQImage(const QSize &size) const;

    uchar *operator()(int x, int y);
    void setPixel(int x, int y, RGB24 color);
    int getDataLength() const;

private:
    constexpr int getDataCnt(int x, int y) const;
    int m_width;
    int m_height;

    uchar *d;
    QImage image;
};

#endif // RAYIMAGE_H
