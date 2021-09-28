
#include <chrono>
#include <time.h>
#include <math.h>

#include "trigo_generator.cpp"

int64_t getTime()
{
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

template<typename T, typename A, typename V>
int64_t execALot(T t, A arg, V &v)
{
    int64_t start = getTime();
    for (int i = 0; i < 1000000; i++)
        t(arg);
    v = t(arg);
    return getTime() - start;
}

void testTrigo()
{
    radian x = 2;

    doubli vCosTradi, vCosCustom;

    int64_t tCosTradi = execALot([](const radian &x) { return cos(x); }, x, vCosTradi);
    int64_t tCosCustom = execALot([](const radian &x) { return myCosTaylorMin(x); }, x, vCosCustom);

    std::cout << "cos Traditionnel : " << tCosTradi << " us, " << vCosTradi << std::endl;
    std::cout << "cos Custom : " << tCosCustom << " us, " << vCosCustom << std::endl;
    std::cout << "Différence du calcul pour x = " << x << " : " << (vCosCustom - vCosTradi) << std::endl;
    if (abs(vCosCustom - vCosTradi) > 1e-6) throw "La différence de calcul est trop grande";
    float optimisation = 1.0 * tCosTradi / tCosCustom;
    if (optimisation > 1) {
        std::cout << "Le cos custom est " << optimisation << " fois plus rapide !";
    } else {
        std::cout << "Le cos custom est " << 1 / optimisation << " fois plus lent :(";
    }
}

void compareTrigo()
{
    int etapes = 100;
    int64_t ttotalCosTradi = 0;
    int64_t ttotalCosCustom = 0;
    for (int i = 0; i <= etapes; i++) {
        const radian x = i * 2 * M_2PI / etapes - M_2PI;
        doubli vCosTradi, vCosCustom;
        int64_t tCosTradi = execALot([](const radian &x) { return cos(x); }, x, vCosTradi);
        int64_t tCosCustom = execALot([](const radian &x) { return myCosTaylorMinAtPI(x); }, x, vCosCustom);
        std::cout << "cos(" << x << ") = " << vCosTradi
                  << ", dt = " << (tCosTradi - tCosCustom) << " us (" << tCosTradi << "-" << tCosCustom << ")" << std::endl;
        if (abs(vCosCustom - vCosTradi) > 1e-6) {
            std::cout << "Différence : " << vCosCustom - vCosTradi << std::endl;
            // le max st 4e-6 avec la précision de 8
        }
        if (abs(vCosCustom - vCosTradi) > 1e-4) {
            throw "La différence de calcul est trop grande";
        }

        ttotalCosTradi += tCosTradi;
        ttotalCosCustom += tCosCustom;
    }
    std::cout << "Temps total : " << ttotalCosTradi << " " << ttotalCosCustom
              << ", " << 1.0 * ttotalCosTradi / ttotalCosCustom << "* plus rapide" << std::endl;
}

int main()
{
    radian x = -M_PI_2;
    //const radian x_trigo = mod(x + M_PI, M_2PI);
    const radian x_trigo = mod(x, M_2PI) - M_PI;
    const radian x2 = x_trigo * x_trigo;
    const doubli ret = myCosTaylorMinAtPI(x);
    std::cout << x << " " << x_trigo << " " << x2 << " " << ret << " " << cos(x) << std::endl;
    // cos(x) = -cos(x+PI)

    //printMycos(7);
    printEffcos(8);

    //testTrigo();
    compareTrigo();

    return 0;
}
