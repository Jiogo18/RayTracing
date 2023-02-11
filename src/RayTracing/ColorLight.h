#ifndef COLORLIGHT_H
#define COLORLIGHT_H

#include "../3D/maths3D.h"
#include "opencv2/core.hpp"

/*****************************************************************************
  ColorLight : la lumière à un certain point de l'espace
  ColorFilter : Filtre des couleurs qui arrivent sur un bloc
  Il faut :
    Une couleur pour les objets traversés, dont l'opacité varie en fonction de la distance traversée
    idée : on peut traverser plusieurs "matériaux" en même temps :
        source de lumière comme une lampe : cercle autour de la lampe, même si on traverse du
        => une couleur de type ColorLight (avec intensité de lumière RGB)
        pour certains matériaux traversés il faut aussi un filtre de couleur
            (pour les torches aussi en faite : fumée)

    Donc :
        les matériaux traversés : ColorLight (lumière RGB) + filtre de couleurs
        les matériaux reflétés : filtre de couleurs
        les matériaux emmisifs : ColorLight (mais aussi filtre de couleurs : champis lumineux par ex?)
            => reflétés et emmisifs en même temps
            => que du ColorLight+filtre
            => on peut faire du overload, mais dans quel sens?
            => non un classe c'est suffisant

    N.B. pour faciliter le calcul : si light > max alors arreter (pb : si lumière encore plus forte derrière mais tant pis)

    lr, lg, lb : bc de light pr le soleil, les sources concentrées (plusieurs Ray)


    TODO: ↑ ColorLight (for light sources) & ColorFilter (for materials) ↑
    faire un tableau par récurrence dans la classe ColorLight pour opti (plutot que le ColorLight)

 *****************************************************************************/

typedef unsigned char uchar;

struct RGB24
{
    uchar r;
    uchar g;
    uchar b;
};

class ColorLight
{
public:
    constexpr inline ColorLight() : r(0), g(0), b(0), a(0), light(0) {}
    constexpr inline ColorLight(const uchar &r, const uchar &g, const uchar &b, const uchar &a, const int &light = 0) : r(r), g(g), b(b), a(a), light(light) {}
    inline ColorLight(const cv::Vec4b &c, const int &light = 0) : r(c[0]), g(c[1]), b(c[2]), a(c[3]), light(light) {}
    constexpr inline ColorLight(const ColorLight &c) : r(c.r), g(c.g), b(c.b), a(c.a), light(c.light) {}
    constexpr inline uchar redReduced(const double &reduce) const { return colorReduced(r, reduce); }
    constexpr inline uchar greenReduced(const double &reduce) const { return colorReduced(g, reduce); }
    constexpr inline uchar blueReduced(const double &reduce) const { return colorReduced(b, reduce); }
    constexpr inline RGB24 getColorReduced(const double &reduce) const;
    constexpr inline const uchar &red() const { return r; }
    constexpr inline const uchar &green() const { return g; }
    constexpr inline const uchar &blue() const { return b; }
    constexpr inline const uchar &alpha() const { return a; }
    constexpr inline const int &getLight() const { return light; }
    constexpr inline void operator+=(const ColorLight &c); // couleur après + couleur avant = couleur mixé
    constexpr inline ColorLight *operator=(const ColorLight &c);

private:
    // entre 0 et 255
    uchar r, g, b, a;
    int light; // entre 0 et infini (0 par défaut et valeur >0 si on arrive à une source de lumière)
    constexpr inline uchar colorReduced(const uchar &c, const double &reduce) const;
};

constexpr inline RGB24 ColorLight::getColorReduced(const double &reduce) const
{
    return RGB24{colorReduced(r, reduce), colorReduced(g, reduce), colorReduced(b, reduce)};
}

constexpr inline void ColorLight::operator+=(const ColorLight &c)
{
    // si this a une luminositée importante (plus que color) alors this est +importante
    // si color a un alpha plus faible (transparent/miroir) alors this est +important
    if (c.a == 0) return; // avant est opaque

    // équivalent :
    // r = r * poidsSuivant + c.r * poidsActuel;
    // g = g * poidsSuivant + c.g * poidsActuel;
    // b = b * poidsSuivant + c.b * poidsActuel;
    r += (-r + c.r) * c.a / 255;
    g += (-g + c.g) * c.a / 255;
    b += (-b + c.b) * c.a / 255;
    // ne prend pas en comtpe la différence de light
    // et peut être buggé sur plusieurs objets transparents à la suite
    // mais tant pis pour le moment

    a = c.a;
    light += c.light;
}

constexpr inline ColorLight *ColorLight::operator=(const ColorLight &c)
{
    r = c.r;
    g = c.g;
    b = c.b;
    a = c.a;
    light = c.light;
    return this;
}

constexpr inline uchar ColorLight::colorReduced(const uchar &c, const double &reduce) const
{
    const double cReduced = c * light / reduce;
    if (255 < cReduced) return 255;
    if (0 < cReduced) return cReduced;
    return 0;
}

#endif // COLORLIGHT_H
