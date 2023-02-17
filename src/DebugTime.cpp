#include "DebugTime.h"
#include <time.h>
#include <chrono>

void DebugTimePart::addValue(int64_t time)
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

void DebugTime::addValue(std::string name, int64_t time)
{
    mutex.lock();
    if (values.find(name) != values.end()) {
        values.insert(std::pair<std::string, DebugTimePart>(name, DebugTimePart()));
    }
    values[name].addValue(time);
    mutex.unlock();
}

int64_t DebugTime::getCurrent()
{
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

int64_t DebugTime::getCurrentMSinceEpoch()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

std::ostream &operator<<(std::ostream &os, const DebugTime &d)
{
    os << "DebugTime {";
    for (std::map<std::string, DebugTimePart>::const_iterator i = d.values.cbegin(); i != d.values.cend(); i++) {
        os << "\n    " << (*i).first
           << " (*" << (*i).second.getNb() << +")"
           << " total:" << (*i).second.getTotal()
           << " moyenne:" << (*i).second.getMoyenne()
           << " min:" << (*i).second.getMin()
           << " max:" << (*i).second.getMax();
    }

    if (!d.getValues().empty()) {
        os << "\n";
    }
    os << "}";
    return os;
}
