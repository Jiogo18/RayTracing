#ifndef COLORLIGHT_H
#define COLORLIGHT_H

#include <QColor>
#include "src/3D/maths3D.h"

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

    // équivalent :
    // r = r * poidsSuivant + c.r * poidsActuel;
    // g = g * poidsSuivant + c.g * poidsActuel;
    // b = b * poidsSuivant + c.b * poidsActuel;
    r += (-r + c.r) * c.a / 255.0;
    g += (-g + c.g) * c.a / 255.0;
    b += (-b + c.b) * c.a / 255.0;
    // ne prend pas en comtpe la différence de light
    // et peut être buggé sur plusieurs objets transparents à la suite
    // mais tant pis pour le moment

    a = c.a;
    light += c.light;
}

constexpr inline int ColorLight::colorReduced(const int &c, const double &reduce) const
{
    return maths3D::max(maths3D::min(c * light / reduce, 255), 0);
}

#endif // COLORLIGHT_H
