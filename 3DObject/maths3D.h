#ifndef MATHS3D_H
#define MATHS3D_H

#include <QtMath>

#define M_2PI 2 * M_PI // 6.28318530717958647692
#define M_1_2PI 1 / (2 * M_PI)
#define DOUBLI_MIN 1e-6

typedef long double doubli; //un nombre pour gérer 6 décimales max (arrondit)

constexpr inline bool isNull(const doubli &d) { return -DOUBLI_MIN < d && d < DOUBLI_MIN; }
constexpr inline doubli roundNull(const doubli &d) { return isNull(d) ? 0 : d; } //plus opti que round
constexpr inline doubli round(const doubli &d) { return (long long int)(d * 1e+6 + 0.5) * DOUBLI_MIN; }
constexpr inline doubli signOf(const doubli &d) { return d < 0 ? -1 : 1; }
constexpr inline doubli sqr(const doubli &d) { return d * d; }
constexpr inline doubli sqrt(const doubli &d) { return std::sqrt(d); }
inline doubli qIsInf(const doubli &d) { return qIsInf(static_cast<double>(d)); }

constexpr inline doubli max(const doubli &a, const doubli &b) { return a > b ? a : b; }
constexpr inline doubli max(const doubli &a, const doubli &b, const doubli &c) { return a > b ? (a > c ? a : c) : (b > c ? b : c); }
constexpr inline doubli min(const doubli &a, const doubli &b) { return a < b ? a : b; }
constexpr inline doubli min(const doubli &a, const doubli &b, const doubli &c) { return a < b ? (a < c ? a : c) : (b < c ? b : c); }

typedef float radian;
inline radian degreesToRadians(const doubli &deg) { return fmodf(deg, 360) * doubli(M_PI) / 180; }
constexpr inline doubli radiansToDegrees(const radian &rad) { return rad * 180 / doubli(M_PI); }
constexpr inline doubli cos(const radian &x) { return std::cos(x); }
constexpr inline doubli sin(const radian &x) { return std::sin(x); }
constexpr inline doubli tan(const radian &x) { return std::tan(x); }
constexpr inline radian acos(const doubli &d) { return std::acos(d); }
constexpr inline radian asin(const doubli &d) { return std::asin(d); }
constexpr inline radian atan(const doubli &d) { return std::atan(d); }

constexpr inline bool isNull(const radian &d) { return -DOUBLI_MIN < d && d < DOUBLI_MIN; }
//perfect return : [0;d[ & 3 times faster
constexpr inline radian mod(const radian &n, const radian &d) { return (long long int)(n / d) * d; }
//intervalle radian : ]-PI; PI]
constexpr inline radian modRad(radian d)
{
	d = mod(d, M_2PI);
	return d > M_PI ? d - M_2PI : d;
}
// plus opti que round
constexpr inline radian roundNull(radian d)
{
	d = modRad(d);
	return isNull(d) ? 0 : d;
}
constexpr inline float signOf(const radian &d) { return modRad(d) < 0 ? -1 : 1; } // -1 or 1

#endif // MATHS3D_H
