#include <QDebug>

#include <chrono>
#include <time.h>
#include <math.h>

#include "trigo_generator.cpp"

QDebug operator<<(QDebug debug, const doubli &d)
{
    debug << static_cast<double>(d);
    return debug;
}

qint64 getTime()
{
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

template<typename T, typename A, typename V>
qint64 execALot(T t, A arg, V &v)
{
    qint64 start = getTime();
    for (int i = 0; i < 1000000; i++)
        t(arg);
    v = t(arg);
    return getTime() - start;
}

void testTrigo()
{
    radian x = 2;

    doubli vCosTradi, vCosCustom;

    qint64 tCosTradi = execALot([](const radian &x) { return cos(x); }, x, vCosTradi);
    qint64 tCosCustom = execALot([](const radian &x) { return myCosTaylorMin(x); }, x, vCosCustom);

    qDebug() << "cos Traditionnel :" << tCosTradi << "us," << vCosTradi;
    qDebug() << "cos Custom :" << tCosCustom << "us," << vCosCustom;
    qDebug() << "Différence du calcul pour x =" << x << ":" << (vCosCustom - vCosTradi);
    if (abs(vCosCustom - vCosTradi) > 1e-6) qFatal("La différence de calcul est trop grande");
    float optimisation = 1.0 * tCosTradi / tCosCustom;
    if (optimisation > 1) {
        qDebug() << "Le cos custom est" << optimisation << "fois plus rapide !";
    } else {
        qDebug() << "Le cos custom est" << 1 / optimisation << "fois plus lent :(";
    }
}

void compareTrigo()
{
    int etapes = 100;
    qint64 ttotalCosTradi = 0;
    qint64 ttotalCosCustom = 0;
    for (int i = 0; i <= etapes; i++) {
        const radian x = i * 2 * M_2PI / etapes - M_2PI;
        doubli vCosTradi, vCosCustom;
        qint64 tCosTradi = execALot([](const radian &x) { return cos(x); }, x, vCosTradi);
        qint64 tCosCustom = execALot([](const radian &x) { return myCosTaylorMinAtPI(x); }, x, vCosCustom);
        qDebug().nospace() << "cos(" << x << ") = " << vCosTradi
                           << ", dt = " << (tCosTradi - tCosCustom) << " us (" << tCosTradi << "-" << tCosCustom << ")";
        if (abs(vCosCustom - vCosTradi) > 1e-6) {
            qDebug() << "Différence : " << vCosCustom - vCosTradi;
            // le max st 4e-6 avec la précision de 8
        }
        if (abs(vCosCustom - vCosTradi) > 1e-4) {
            qFatal("La différence de calcul est trop grande");
        }

        ttotalCosTradi += tCosTradi;
        ttotalCosCustom += tCosCustom;
    }
    qDebug() << "Temps total :" << ttotalCosTradi << ttotalCosCustom
             << "," << 1.0 * ttotalCosTradi / ttotalCosCustom << "* plus rapide";
}

int main()
{
    radian x = -M_PI_2;
    //const radian x_trigo = mod(x + M_PI, M_2PI);
    const radian x_trigo = mod(x, M_2PI) - M_PI;
    const radian x2 = x_trigo * x_trigo;
    const doubli ret = myCosTaylorMinAtPI(x);
    qDebug() << x << x_trigo << x2 << ret << cos(x);
    // cos(x) = -cos(x+PI)

    //printMycos(7);
    printEffcos(8);

    //testTrigo();
    compareTrigo();

    return 0;
}
