#ifndef COLORLIGHT_H
#define COLORLIGHT_H

#include <QColor>
#include "src/3D/maths3D.h"

class ColorLight
{
public:
    constexpr inline ColorLight() : r(0), g(0), b(0), a(0), light(0) {}
    constexpr inline ColorLight(const int &r, const int &g, const int &b, const int &a, const int &light = 0) : r(r), g(g), b(b), a(a > 255 ? 255 : a), light(light) {}
    inline ColorLight(const QColor &c, const int &light = 0) : r(c.red()), g(c.green()), b(c.blue()), a(maths3D::min(c.alpha(), 255)), light(light) {}
    constexpr inline ColorLight(const ColorLight &c) : r(c.r), g(c.g), b(c.b), a(c.a), light(c.light) {}
    constexpr inline QColor getColorReduced(const double &reduce) const;
    constexpr inline int red() const { return r; }
    constexpr inline int green() const { return g; }
    constexpr inline int blue() const { return b; }
    constexpr inline int alpha() const { return a; }
    constexpr inline int getLight() const { return light; }
    constexpr inline void operator+=(const ColorLight &c); //couleur après + couleur avant = couleur mixé
    ColorLight *operator=(const ColorLight &c);

private:
    //entre 0 et 255
    int r, g, b, a;
    int light; //entre 0 et infini (0 par défaut et valeur >0 si on arrive à une source de lumière)
    //bc de light pr le soleil, les sources concentrées (plusieurs Ray)
    constexpr inline int colorReduced(const int &c, const double &reduce) const;
};

constexpr inline QColor ColorLight::getColorReduced(const double &reduce) const
{
    return QColor{colorReduced(r, reduce), colorReduced(g, reduce), colorReduced(b, reduce)};
}

constexpr inline void ColorLight::operator+=(const ColorLight &c)
{
    // si this a une luminositée importante (plus que color) alors this est +importante
    // si color a un alpha plus faible (transparent/miroir) alors this est +important
    if (c.a == 0) return; // avant est opaque

    double poids = (light - c.light + 1.0) * 255.0 / c.a;

    r = (r * poids + c.r) / (1 + poids);
    g = (g * poids + c.g) / (1 + poids);
    b = (b * poids + c.b) / (1 + poids);
    a = maths3D::min(a + c.a, 255);
    light += c.light;
}

constexpr inline int ColorLight::colorReduced(const int &c, const double &reduce) const
{
    return maths3D::min(c * light / reduce, 255);
}

// TODO: ColorLight (for light sources) & ColorFilter (for materials)

#endif // COLORLIGHT_H
