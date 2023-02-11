#include "RayImage.h"

#define ImageFormat Image::Format_RGBA32

RayImage::RayImage() : RayImage(1, 1) {}

RayImage::RayImage(int width, int height) : Image(width, height, ImageFormat)
{
    fill(RGBA(0, 0, 0, 0));
}

RayImage::RayImage(const QSize &size) : RayImage(size.width(), size.height()) {}
