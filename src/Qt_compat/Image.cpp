#include "Image.h"

Image::Image() : widthp(0), heightp(0)
{
    int pixels = getPixels();
    red = new uchar[pixels];
    green = new uchar[pixels];
    blue = new uchar[pixels];
}

Image::Image(int width, int height, Image::Format format) : widthp(width), heightp(height)
{
    int pixels = getPixels();
    red = new uchar[pixels];
    green = new uchar[pixels];
    blue = new uchar[pixels];
}

Image::Image(const QSize &size) : widthp(size.width()), heightp(size.height())
{
    int pixels = getPixels();
    red = new uchar[pixels];
    green = new uchar[pixels];
    blue = new uchar[pixels];
}

Image::Image(const uchar *data, int width, int height, Image::Format format) : widthp(width), heightp(height)
{
    int pixels = getPixels();
    red = new uchar[pixels];
    green = new uchar[pixels];
    blue = new uchar[pixels];
}

Image::Image(const Image &image) : widthp(image.widthp), heightp(image.heightp)
{
    int pixels = getPixels();
    red = new uchar[pixels];
    green = new uchar[pixels];
    blue = new uchar[pixels];
    memcpy(&red[0], &image.red[0], pixels * sizeof(uchar));
    memcpy(&green[0], &image.green[0], pixels * sizeof(uchar));
    memcpy(&blue[0], &image.blue[0], pixels * sizeof(uchar));
}

Image::~Image()
{
    if (red) delete[] red;
    if (green) delete[] green;
    if (blue) delete[] blue;
}

Image &Image::operator=(const Image &image)
{
    widthp = image.widthp;
    heightp = image.heightp;
    int pixels = getPixels();
    red = new uchar[pixels];
    green = new uchar[pixels];
    blue = new uchar[pixels];
    memcpy(&red[0], &image.red[0], pixels * sizeof(uchar));
    memcpy(&green[0], &image.green[0], pixels * sizeof(uchar));
    memcpy(&blue[0], &image.blue[0], pixels * sizeof(uchar));
    return *this;
}

Image Image::scaled(const QSize &size) const
{
    return Image(size);
}

QColor Image::pixelColor(int x, int y) const
{
    int pixel = x + y * widthp;
    return QColor(red[pixel], green[pixel], blue[pixel]);
}

COLORREF Image::pixelRGB(int x, int y) const
{
    return RGB(red[x], green[x], blue[x]);
}

void Image::fill(QColor color)
{
    int pixels = getPixels();
    std::fill(&red[0], &red[pixels], color.red());
    std::fill(&green[0], &green[pixels], color.green());
    std::fill(&blue[0], &blue[pixels], color.blue());
}

void Image::setPixelColor(int x, int y, QColor color)
{
    int pixel = x + y * widthp;
    red[pixel] = color.red();
    green[pixel] = color.green();
    blue[pixel] = color.blue();
}
