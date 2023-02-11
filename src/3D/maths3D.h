#ifndef MATHS3D_H
#define MATHS3D_H

#include <math.h>
#include <iostream>
#include "../utils/global.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif // M_PI
#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif // M_PI_2
#define M_2PI 6.28318530717958647692
#define M_1_2PI 0.15915494309189533576901767873386
#define DOUBLI_MIN 1e-10

namespace maths3D {
    typedef long double doubli; // un nombre pour gérer 6 décimales max (arrondit)
    const float signPlus = 1;
    const float signMoins = -1;
    const doubli nul = 0;

    constexpr inline bool isNull(const doubli &d) { return -DOUBLI_MIN < d && d < DOUBLI_MIN; }
    constexpr inline const doubli &roundNull(const doubli &d) { return isNull(d) ? nul : d; } // plus opti que round
    constexpr inline const float &signOf(const doubli &d) { return d < 0 ? signMoins : signPlus; }
    constexpr inline doubli roundDoubli(const doubli &d) { return (long long int)(d / DOUBLI_MIN + signOf(d) * 0.5) * DOUBLI_MIN; }
    constexpr inline doubli sqr(const doubli &d) { return d * d; }
    inline doubli sqrtCExpr(const doubli &d) { return std::sqrt(d); }
    inline doubli distanceLoxodromique(const doubli &a, const doubli &b) { return sqrtCExpr(a * a + b * b); }
    inline doubli distanceLoxodromique(const doubli &a, const doubli &b, const doubli &c) { return sqrtCExpr(a * a + b * b + c * c); }
// les fonctions define sont plus rapide (2*) mais empchent d'avoir le constexpr
#define sqrD(x) ((x) * (x))
#define distanceLoxodromique_2(a, b) (sqrtf(sqrD(a) + sqrD(b)))
#define distanceLoxodromique_3(a, b, c) (sqrtf(sqrD(a) + sqrD(b) + sqrD(c)))

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MAX_3(a, b, c) (MAX(MAX(a, b), c))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MIN_3(a, b, c) (MIN(MIN(a, b), c))
#define ABS(x) ((x) < 0 ? -(x) : (x))
#define MAX_ABS_3(a, b, c) (MAX_3(ABS(a), ABS(b), ABS(c)))
    //

    constexpr const doubli &max(const doubli &a, const doubli &b) { return a > b ? a : b; }
    constexpr const doubli &max(const doubli &a, const doubli &b, const doubli &c) { return a > b ? (a > c ? a : c) : (b > c ? b : c); }
    constexpr const doubli &min(const doubli &a, const doubli &b) { return a < b ? a : b; }
    constexpr const doubli &min(const doubli &a, const doubli &b, const doubli &c) { return a < b ? (a < c ? a : c) : (b < c ? b : c); }

    typedef double radian;
    inline radian degreesToRadians(const doubli &deg) { return fmodf(deg, 360) * doubli(M_PI) / 180; }
    constexpr inline doubli radiansToDegrees(const radian &rad) { return rad * 180 / doubli(M_PI); }

    constexpr inline bool isNull(const radian &d) { return -DOUBLI_MIN < d && d < DOUBLI_MIN; }
    // perfect return : [0;d[ & 3 times faster
    constexpr inline radian mod(const radian &n, const radian &d) { return n - ((long long int)(n / d) - (n < 0)) * d; }
    // intervalle radian : ]-PI; PI]
    constexpr inline radian modRad(radian d)
    {
        d = mod(d, M_2PI);
        return d > M_PI ? d - M_2PI : d;
    }
    // plus opti que round :
    constexpr inline radian roundNull(radian d)
    {
        d = mod(d, M_2PI);
        return isNull(d) ? 0 : d;
    }
    constexpr inline const float &signOf(const radian &d) { return (modRad(d) < 0) ? signMoins : signPlus; }

    /*****************************************************************************
      Versions optimisées des formules de trigonométrie
      Utilise le développement de Taylor de façon à optimiser les calculs
      Les calculs sont 1.7 fois plus rapides avec une précision à 5 décimales
      Voir tests/trigo https://github.com/Jiogo18/RayTracing/tree/master/tests/trigo
     *****************************************************************************/

#define trigoTaylorLevel 11
    constexpr inline doubli cosTaylorMin(const radian &x)
    {
        const radian x_trigo = mod(x, M_2PI) - M_PI; // X = x+-M_PI => X sur [-PI;PI[
        const radian x2 = x_trigo * x_trigo;
#if trigoTaylorLevel <= 10
        return -1 + x2 * (1.0 / 2 - x2 * (1.0 / 24 - x2 * (1.0 / 720 - x2 * (1.0 / 40320 - x2 * (1.0 / 3628800)))));
#elif trigoTaylorLevel <= 12
        return -1 + x2 * (1.0 / 2 - x2 * (1.0 / 24 - x2 * (1.0 / 720 - x2 * (1.0 / 40320 - x2 * (1.0 / 3628800 - x2 * (1.0 / 479001600))))));
#else // if trigoTaylorLevel <= 14
        return -1 + x2 * (1.0 / 2 - x2 * (1.0 / 24 - x2 * (1.0 / 720 - x2 * (1.0 / 40320 - x2 * (1.0 / 3628800 - x2 * (1.0 / 479001600 - x2 * (1.0 / 87178291200)))))));
#endif
    }
    constexpr inline doubli sinTaylorMin(const radian &x)
    {
        const radian x_trigo = mod(x, M_2PI) - M_PI; // X = x+-M_PI => X sur [-PI;PI[
        const radian x2 = x_trigo * x_trigo;
#if trigoTaylorLevel <= 9
        return x_trigo * (-1 + x2 * (1.0 / 6 - x2 * (1.0 / 120 - x2 * (1.0 / 5040 - x2 * (1.0 / 362880)))));
#elif trigoTaylorLevel <= 11
        return x_trigo * (-1 + x2 * (1.0 / 6 - x2 * (1.0 / 120 - x2 * (1.0 / 5040 - x2 * (1.0 / 362880 - x2 * (1.0 / 39916800))))));
#elif trigoTaylorLevel <= 13
        return x_trigo * (-1 + x2 * (1.0 / 6 - x2 * (1.0 / 120 - x2 * (1.0 / 5040 - x2 * (1.0 / 362880 - x2 * (1.0 / 39916800 - x2 * (1.0 / 6227020800)))))));
#else
        return x_trigo * (-1 + x2 * (1.0 / 6 - x2 * (1.0 / 120 - x2 * (1.0 / 5040 - x2 * (1.0 / 362880 - x2 * (1.0 / 39916800 - x2 * (1.0 / 6227020800 - x2 * (1.0 / 1307674368000))))))));
#endif
    }
    constexpr inline doubli tanTaylor(const radian &x)
    {
        const radian x_trigo = mod(x + M_PI, M_2PI) - M_PI; // X sur [-PI;PI[
        const radian x2 = x_trigo * x_trigo;
#if trigoTaylorLevel <= 9
        return x_trigo * (1 + x2 * (1.0 / 3 + x2 * (2.0 / 15 + x2 * (17.0 / 315 + x2 * (62.0 / 2835)))));
#elif trigoTaylorLevel <= 11
        return x_trigo * (1 + x2 * (1.0 / 3 + x2 * (2.0 / 15 + x2 * (17.0 / 315 + x2 * (62.0 / 2835 + x2 * (1382.0 / 155925))))));
#elif trigoTaylorLevel <= 13
        return x_trigo * (1 + x2 * (1.0 / 3 + x2 * (2.0 / 15 + x2 * (17.0 / 315 + x2 * (62.0 / 2835 + x2 * (1382.0 / 155925 + x2 * (21844.0 / 6081075)))))));
#else
        return x_trigo * (1 + x2 * (1.0 / 3 + x2 * (2.0 / 15 + x2 * (17.0 / 315 + x2 * (62.0 / 2835 + x2 * (1382.0 / 155925 + x2 * (21844.0 / 6081075 - x2 * (929569.0 / 638512875))))))));
#endif
    }
    constexpr inline radian atanSimple(const doubli &x)
    {
        if (x >= 4) {
            return M_PI_2 - 1 / x;
        } else if (x <= -4) {
            return -M_PI_2 - 1 / x;
        } else if (x >= 2) {
            return -0.0326 * x * x + 0.3047 * x + 0.6280;
        } else if (x <= -2) {
            return 0.0326 * x * x + 0.3047 * x - 0.6280;
        } else if (x >= 1) {
            return -0.1461 * x * x + 0.7600 * x + 0.1715;
        } else if (x <= -1) {
            return 0.1461 * x * x + 0.7600 * x - 0.1715;
        } else if (x > 0) {
            return -0.2838 * x * x + 1.0692 * x;
        } else if (x < 0) {
            return 0.2838 * x * x + 1.0692 * x;
        } else {
            return 0;
        }
    }

} // namespace maths3D

#endif // MATHS3D_H
