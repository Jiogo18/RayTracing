#include <iostream>
#include <math.h>

typedef long double doubli;
typedef double radian;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif // M_PI
#ifndef M_PI_2
#define M_PI_2 1.57079632679489661923
#endif // M_PI_2
#define M_2PI 2 * M_PI

// perfect return : [0;d[ & 3 times faster
constexpr inline radian mod(const radian &n, const radian &d) { return n - ((long long int)(n / d) - (n < 0)) * d; }
// intervalle radian : ]-PI; PI]
constexpr inline radian modRad(radian d)
{
    d = mod(d, M_2PI);
    return d > M_PI ? d - M_2PI : d;
}

constexpr inline doubli myCosTaylor(const radian &x)
{
    double s = 1;
    const radian x_trigo = modRad(x);
    const radian x2 = x_trigo * x_trigo;
    radian fac = 1;
    for (int k = 2; k < 14; k += 2) {
        fac *= -x2 / ((k - 1) * k);
        s += fac;
    }
    return s;
}

constexpr inline doubli myCosTaylorMin(radian x)
{
    const radian x_trigo = modRad(x);
    const radian x2 = x_trigo * x_trigo;
    return 1 - x2 * (1.0 / 2 - x2 * (1.0 / 24 - x2 * (1.0 / 720 - x2 * (1.0 / 40320 - x2 * (1.0 / 3.6288e+6 - x2 * (1.0 / 4.79002e8 - x2 * (1.0 / 8.71783e10)))))));
}

constexpr inline doubli myCosTaylorAtPI(const radian &x)
{
    double s = 1;
    const radian x_trigo = mod(x, M_2PI) - M_PI;
    const radian x2 = x_trigo * x_trigo;
    radian fac = 1;
    for (int k = 2; k < 16; k += 2) {
        fac *= -x2 / ((k - 1) * k);
        s += fac;
    }
    return -s;
}

constexpr inline doubli myCosTaylorMinAtPI(const radian &x)
{
    // Développement de Taylor en PI ( pour être centré sur l'intervalle [0;2PI[ )
    // Utilise celui en 0 avec : cos(x) = -cos(x+PI) = -cos(X)
    const radian x_trigo = mod(x, M_2PI) - M_PI; // X = x+-M_PI et X sur [-PI;PI[
    const radian x2 = x_trigo * x_trigo;
    return -1 + x2 * (1.0 / 2 - x2 * (1.0 / 24 - x2 * (1.0 / 720 - x2 * (1.0 / 40320 - x2 * (1.0 / 3628800 - x2 * (1.0 / 479001600 - x2 * (1.0 / 87178291200)))))));

    // créées par printEffcos (il faut juste mettre -1 + x2 au début car c'est -cos)
    // il n'y pas l'air de changement apparant entre ces 2 résultats mais le 2e est plus compact
    // ou sinon calculer à la main les termes comme à la première
    //return - 1+x2 * (1.0/2-x2 * (1.0/24-x2 * (1.0/720-x2 * (1.0/40320 - x2 * (1.0/3.6288e+6 -x2 * (1.0/4.79002e8-x2 * (1.0/8.71783e10)))))));
    //return -1 + x2 * (0.5 - x2 * (0.0416667 - x2 * (0.00138889 - x2 * (2.48016e-05 - x2 * (2.75573e-07 - x2 * (2.08768e-09 - x2 * (1.14707e-11)))))));
    // cos(x) = -cos(x+PI) = -cos(X)
}

void printEffcos(int precision)
{
    std::cout << "1";
    double fac = 1;
    int k = 2;
    for (k = 2; k < precision * 2; k += 2) {
        fac *= (k - 1) * k;
        //std::cout << "-x^2 * (1.0/(" << fac << ")";
        std::cout << "-x2 * (" << 1 / fac;
    }
    for (; k >= 2; k -= 2)
        std::cout << ")";
    std::cout << std::endl;
}

void printCosTaylor(int precision)
{
    std::cout << "1";
    double fac = 1;
    for (int k = 2; k < precision * 2; k += 2) {
        fac *= -(k - 1) * k;
        if (fac < 0)
            std::cout << "-(x^" << k << ")/(" << -fac << ")";
        else
            std::cout << "+(x^" << k << ")/(" << fac << ")";
    }
    std::cout << std::endl;
}

constexpr inline doubli mySinTaylorMinAtPI(const radian &x)
{
    // On pourrait utiliser le DT de cos mais c'est plus lent
    const radian x_trigo = mod(x, M_2PI) - M_PI; // X = x+-M_PI et X sur [-PI;PI[
    const radian x2 = x_trigo * x_trigo;
    return x_trigo * (-1 + x2 * (1.0 / 6 - x2 * (1.0 / 120 - x2 * (1.0 / 5040 - x2 * (1.0 / 362880 - x2 * (1.0 / 39916800 - x2 * (1.0 / 6227020800 - x2 * (1.0 / 1307674368000))))))));

    //return x_trigo * (-1 + x2 * (0.16666666667 - x2 * (0.008333333333 - x2 * (1.984126984e-4 - x2 * (2.755731922399e-06 - x2 * (2.50521083854e-08 - x2 * (1.6059043836821e-10 - x2 * (7.6471637318e-13))))))));
    // sin(x) = -sin(x+PI) = -sin(X)
}
