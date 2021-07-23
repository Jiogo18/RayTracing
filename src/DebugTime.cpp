#include "DebugTime.h"

void DebugTimePart::addValue(qint64 time)
{
    if (nb == 0) {
        min = time;
        max = time;
    }
    total += time;
    nb++;
    if (time < min) min = time;
    if (max < time) max = time;
}

void DebugTime::addValue(QString name, qint64 time)
{
    if (!values.contains(name)) {
        values.insert(name, DebugTimePart());
    }
    values[name].addValue(time);
}

qint64 DebugTime::getCurrent()
{
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

QDebug operator<<(QDebug debug, const DebugTime &d)
{
    debug << "DebugTime {";
    QMapIterator<QString, DebugTimePart> i(d.getValues());
    while (i.hasNext()) {
        i.next();
        debug << "\n    " << i.key()
              << QString("(*" + QString::number(i.value().getNb()) + ")").toStdString().c_str()
              << "total:" << i.value().getTotal()
              << "moyenne:" << i.value().getMoyenne()
              << "min:" << i.value().getMin()
              << "max:" << i.value().getMax();
    }
    if (!d.getValues().isEmpty()) {
        debug << "\n";
    }
    debug << "}";
    return debug;
}
