#pragma once

#include <functional>
#include <thread>
#include <chrono>
#include <time.h>

class Thread
{
public:
    Thread();
    ~Thread();

    static void sleepMs(int duration);
    static int getCurrentMsSinceEpoch();
    static int getCurrentMsSinceLocal();

    void connectOnFinished(std::function<void()> callback);
    bool isRunning() const { return running && thread != nullptr; }

    void start();
    void join();
    void stop() { stop_asked = true; }
    void quit() { stop_asked = true; }
    void terminate();
    void wait(int time);

protected:
    virtual void run() = 0;
    bool stop_asked = false;

private:
    std::thread *thread = nullptr;
    bool running = false;
    std::function<void()> callbackFunction;

    void startInternalProcess();
};
