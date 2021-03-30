#ifndef SPEEDDEBUG_CPP
#define SPEEDDEBUG_CPP

#include <QDebug>
#include <time.h>
#include <chrono>

qint64 getTime()
{
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

void debugVerifTime(QString name, qint64 start, qint64 timeMin, qint64 timeMax)
{
    qint64 time = getTime() - start;
    if (time < timeMin || timeMax < time) {
        qDebug() << name << "in" << time << "ms (contre" << (timeMin + timeMax) / 2 << "moyen)";
    }
}

template<typename T>
qint64 verifTime(T t, int repeat = 1)
{
    qint64 start = getTime();
    for (int i = 0; i < 1000000; i++) {
        for (int i2 = 0; i2 < repeat; i2++) {
            t();
        }
    }
    return start;
}
template<typename T, typename A>
qint64 verifTime_1(T t, A a, int repeat = 1)
{
    qint64 start = getTime();
    for (int i = 0; i < 1000000; i++) {
        for (int i2 = 0; i2 < repeat; i2++) {
            t(a);
        }
    }
    return start;
}
template<typename T, typename A, typename B>
qint64 verifTime_2(T t, A a, B b, int repeat = 1)
{
    qint64 start = getTime();
    for (int i = 0; i < 1000000; i++) {
        for (int i2 = 0; i2 < repeat; i2++) {
            t(a, b);
        }
    }
    return start;
}

#endif // SPEEDDEBUG_CPP
