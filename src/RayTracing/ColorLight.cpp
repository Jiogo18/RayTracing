#include "ColorLight.h"

//void ColorLight::operator+=(const ColorLight &color)
//{
//    // si this a une luminositée importante (plus que color) alors this est +importante
//    // si color a un alpha plus faible (transparent/miroir) alors this est +important
//    if (color.a == 0) return; // avant est opaque

//    double poids = (light - color.light + 1.0) * 255.0 / color.a;

//    r = (r * poids + color.r) / (1 + poids);
//    g = (g * poids + color.g) / (1 + poids);
//    b = (b * poids + color.b) / (1 + poids);
//    a = std::min(a + color.a, 255);
//    light += color.light;
//}

ColorLight *ColorLight::operator=(const ColorLight &c)
{
    r = c.r;
    g = c.g;
    b = c.b;
    a = c.a;
    light = c.light;
    return this;
}
