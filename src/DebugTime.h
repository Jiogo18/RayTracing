#ifndef DEBUG_H
#define DEBUG_H

#include <map>
#include <iostream>
#include <string>
#include <mutex>

class DebugTimePart
{
public:
    DebugTimePart()
    {}
    void addValue(int64_t time);
    int64_t getMin() const { return min; }
    int64_t getMax() const { return max; }
    int64_t getNb() const { return nb; }
    int64_t getTotal() const { return total; }
    int64_t getMoyenne() const { return nb == 0 ? 0 : total / nb; }

private:
    int64_t min = 0;
    int64_t max = 0;
    int64_t total = 0;
    int64_t nb = 0;
};

class DebugTime
{
public:
    DebugTime() {}
    void addValue(std::string name, int64_t time);
    std::map<std::string, DebugTimePart> getValues() const { return values; }
    static int64_t getCurrent();
    static int64_t getCurrentMSinceEpoch();
    void clear() { values.clear(); }

    friend std::ostream &operator<<(std::ostream &os, const DebugTime &d);

private:
    std::map<std::string, DebugTimePart> values;
    std::mutex mutex;
};

#endif // DEBUG_H
