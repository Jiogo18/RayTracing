#ifndef MATHS3D_H
#define MATHS3D_H

#include <QtMath>

#define M_2PI 2 * M_PI // 6.28318530717958647692
#define M_1_2PI 1 / (2 * M_PI)
#define DOUBLI_MIN 1e-6

namespace maths3D {
    typedef long double doubli; // un nombre pour gérer 6 décimales max (arrondit)

    constexpr inline bool isNull(const doubli &d) { return -DOUBLI_MIN < d && d < DOUBLI_MIN; }
    constexpr inline doubli roundNull(const doubli d) { return isNull(d) ? 0 : d; } // plus opti que round
    constexpr inline doubli round(const doubli &d) { return (long long int)(d * 1e+6 + 0.5) * DOUBLI_MIN; }
    constexpr inline doubli signOf(const doubli &d) { return d < 0 ? -1 : 1; }
    constexpr inline doubli sqr(const doubli &d) { return d * d; }
    constexpr inline doubli sqrtCExpr(const doubli &d) { return std::sqrt(d); }
    constexpr inline doubli distanceLoxodromique(const doubli &a, const doubli &b) { return sqrtCExpr(a * a + b * b); }
    constexpr inline doubli distanceLoxodromique(const doubli &a, const doubli &b, const doubli &c) { return sqrtCExpr(a * a + b * b + c * c); }
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

    constexpr doubli max(const doubli &a, const doubli &b) { return a > b ? a : b; }
    constexpr doubli max(const doubli &a, const doubli &b, const doubli &c) { return a > b ? (a > c ? a : c) : (b > c ? b : c); }
    constexpr doubli min(const doubli &a, const doubli &b) { return a < b ? a : b; }
    constexpr doubli min(const doubli &a, const doubli &b, const doubli &c) { return a < b ? (a < c ? a : c) : (b < c ? b : c); }

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
        d = modRad(d);
        return isNull(d) ? 0 : d;
    }
    constexpr inline float signOf(const radian &d) { return modRad(d) < 0 ? -1 : 1; } // -1 or 1

    /*****************************************************************************
      Versions optimisées des formules de trigonométrie
      Utilise le développement de Taylor de façon à optimiser les calculs
      Les calculs sont 1.7 fois plus rapides avec une précision à 5 décimales
      Voir tests/trigo https://github.com/Jiogo18/RayTracing/tree/master/tests/trigo
     *****************************************************************************/

#define trigoTaylorLevel 9
    constexpr inline doubli cosTaylorMin(const radian &x)
    {
        const radian x_trigo = mod(x, M_2PI) - M_PI; // X = x+-M_PI => X sur [-PI;PI[
        const radian x2 = x_trigo * x_trigo;
#if trigoTaylorLevel <= 10
        return -1 + x2 * (1.0 / 2 - x2 * (1.0 / 24 - x2 * (1.0 / 720 - x2 * (1.0 / 40320 - x2 * (1.0 / 3628800)))));
#elif trigoTaylorLevel <= 12
        return -1 + x2 * (1.0 / 2 - x2 * (1.0 / 24 - x2 * (1.0 / 720 - x2 * (1.0 / 40320 - x2 * (1.0 / 3628800 - x2 * (1.0 / 479001600))))));
#else //if trigoTaylorLevel <= 14
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

} // namespace maths3D

#endif // MATHS3D_H
