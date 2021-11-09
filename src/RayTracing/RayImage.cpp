#include "RayImage.h"

#define charPerPixel 4
#define ImageFormat Image::Format_RGB32

RayImage::RayImage() : RayImage(1, 1) {}

RayImage::RayImage(int width, int height) : m_width(width),
                                            m_height(height),
                                            d(new uchar[getDataLength()]),
                                            image(d, m_width, m_height, ImageFormat)
{
    for (int cnt = 0; cnt < getDataLength();) {
        d[cnt] = 0;
        cnt++;
        d[cnt] = 0;
        cnt++;
        d[cnt] = 0;
        cnt++;        // Black
        d[cnt] = 255; // Alpha
        cnt++;
    }
}

RayImage::RayImage(const QSize &size) : RayImage(size.width(), size.height()) {}

RayImage::RayImage(const RayImage &that) : m_width(that.m_width),
                                           m_height(that.m_height),
                                           d(new uchar[that.getDataLength()]),
                                           image(d, m_width, m_height, ImageFormat)
{
    memcpy(d, that.d, that.getDataLength() * sizeof(uchar));
}
RayImage::~RayImage()
{
    delete[] d;
    d = nullptr;
}

RayImage &RayImage::operator=(const RayImage &that)
{
    if (m_width != that.m_width || m_height != that.m_height) {
        m_width = that.m_width;
        m_height = that.m_height;

        delete[] d;
        d = new uchar[that.getDataLength()];
        image = Image(d, m_width, m_height, ImageFormat);
    }

    memcpy(d, that.d, that.getDataLength() * sizeof(uchar));

    return *this;
}

RayImage RayImage::scaled(const QSize &size) const
{
    double scale_x = size.width() / m_width;
    double scale_y = size.height() / m_height;
    RayImage image(size);
    int scaledX;
    int scaledY;
    int scaledCnt;
    int realX;
    int realY;
    int realCnt;
    for (scaledX = 0; scaledX < image.m_width; scaledX++) {
        realX = std::floor(scaledX / scale_x);
        for (scaledY = 0; scaledY < image.m_height; scaledY++) {
            realY = std::floor(scaledY / scale_y);
            scaledCnt = getDataCnt(scaledX, scaledY);
            realCnt = getDataCnt(realX, realY);
            image.d[scaledCnt] = d[realCnt];
            image.d[scaledCnt + 1] = d[realCnt + 1];
            image.d[scaledCnt + 2] = d[realCnt + 2];
        }
    }
    return image;
}

Image RayImage::toImage() const
{
    return image;
}
Image RayImage::toImage(const QSize &size) const
{
    return image.scaled(size);
}

void RayImage::fillBitmapPixels(BYTE *bytes, int size) const
{
    int s = MIN(size, 4 * m_width * m_height);
    memcpy(bytes, d, sizeof(BYTE) * s);
}

uchar *RayImage::operator()(int x, int y)
{
    ASSERT(0 <= x && x < m_width && 0 <= y && y < m_height);
    return &d[getDataCnt(x, y)];
}

void RayImage::setPixel(int x, int y, RGB24 color)
{
    ASSERT(0 <= x && x < m_width && 0 <= y && y < m_height);
    uchar *p = &d[getDataCnt(x, y)];
    *p++ = color.b;
    *p++ = color.g;
    *p = color.r;
}

RGB24 RayImage::getPixel(int x, int y) const
{
    ASSERT(0 <= x && x < m_width && 0 <= y && y < m_height);
    uchar *p = &d[getDataCnt(x, y)];
    uchar b = *p++;
    uchar g = *p++;
    uchar r = *p;
    return RGB24{r, g, b};
}

int RayImage::getDataLength() const
{
    return m_width * m_height * charPerPixel;
}

constexpr int RayImage::getDataCnt(int x, int y) const
{
    return (x + (m_width)*y) * charPerPixel;
}
