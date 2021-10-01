#ifndef RAYIMAGE_H
#define RAYIMAGE_H

#include "ColorLight.h"
#include "../Qt_compat/Image.h"
#include "../Qt_compat/QPixmap.h"

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
    Image toImage() const;
    Image toImage(const QSize &size) const;

    uchar *operator()(int x, int y);
    void setPixel(int x, int y, RGB24 color);
    int getDataLength() const;

    int width() const { return m_width; }
    int height() const { return m_height; }
    QSize size() const { return QSize(m_width, m_height); }

private:
    constexpr int getDataCnt(int x, int y) const;
    int m_width;
    int m_height;

    uchar *d;
    Image image;
};

#endif // RAYIMAGE_H
