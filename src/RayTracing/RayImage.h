#ifndef RAYIMAGE_H
#define RAYIMAGE_H

#include "ColorLight.h"
#include "../Qt_compat/Image.h"

class RayImage : public Image
{
public:
    RayImage();
    RayImage(int width, int height);
    RayImage(const QSize &size);
};

#endif // RAYIMAGE_H
