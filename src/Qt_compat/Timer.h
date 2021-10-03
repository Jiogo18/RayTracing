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
    int interval = 0;
    std::function<void()> timeoutFunction;
};

class SingleShotTimer : public Thread
{
public:
    SingleShotTimer() {}

    void start()
    {
        end_time = getCurrentMsSinceLocal() + interval;
        if (!isRunning())
            Thread::start();
    }
    void start(int interval)
    {
        this->interval = interval;
        end_time = getCurrentMsSinceLocal() + interval;
        if (!isRunning())
            Thread::start();
    }
    void setInterval(int interval) { this->interval = interval; }
    void stop()
    {
        Thread::stop();
    }

protected:
    void run()
    {
        int remaning;
        while ((remaning = end_time - getCurrentMsSinceLocal()) > 0) {
            sleepMs(remaning); // Sleep until you reach 'end_time'
        }
    }

private:
    int interval = 0;
    int end_time;
};