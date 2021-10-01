#pragma once

#include "Thread.h"

class Timer : public Thread
{
public:
    Timer() {}

    void start()
    {
        Thread::start();
    }
    void start(int interval)
    {
        this->interval = interval;
        Thread::start();
    }
    void setInterval(int interval) { this->interval = interval; }
    void setSingleShot(bool single_shot) { this->single_shot = single_shot; }
    void stop()
    {
        Thread::stop();
    }

    void connectOnTimeout(std::function<void()> timeoutFunction) { this->timeoutFunction = timeoutFunction; }

protected:
    void run()
    {
        do {
            sleepMs(interval);
            timeoutFunction();
        } while (!single_shot && !stop_asked);
    }

private:
    bool single_shot = false;
    int interval;
    std::function<void()> timeoutFunction;
};