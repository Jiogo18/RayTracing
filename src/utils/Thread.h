#pragma once

#include <functional>
#include <thread>
#include <chrono>
#include <time.h>
#include <mutex>
#include <condition_variable>

class Thread
{
public:
    Thread();
    ~Thread();

    static void sleepMs(int duration);
    static int getCurrentMsSinceEpoch();
    static int getCurrentMsSinceLocal();

    void connectOnFinished(std::function<void()> callback);
    bool isRunning() const { return running; }

    void start();
    void stop();
    void quit();
    void terminate();
    void wait(int time);

protected:
    virtual void run() = 0;
    bool stop_asked = false;

private:
    std::thread *thread = nullptr;
    bool running = false;
    std::function<void()> callbackFunction;

    bool stopAtNextFinish = false;
    std::mutex mutexPause;
    std::condition_variable condPause;
    std::unique_lock<std::mutex> lockPause;

    void startInternalProcess();
};
