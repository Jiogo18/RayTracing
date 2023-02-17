#include "Thread.h"

Thread::Thread()
{
    lockPause = std::unique_lock<std::mutex>(mutexPause);
}

Thread::~Thread()
{
    if (thread != nullptr) delete thread;
}

void Thread::sleepMs(int duration)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(duration));
}

int Thread::getCurrentMsSinceEpoch()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
}

int Thread::getCurrentMsSinceLocal()
{
    return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
}

void Thread::connectOnFinished(std::function<void()> callback)
{
    callbackFunction = callback;
}

void Thread::start()
{
    stop_asked = false;
    stopAtNextFinish = false;
    if (thread != nullptr) {
        if (running) {
            throw std::runtime_error("Thread already running");
        }
        condPause.notify_one(); // notify to continue
    } else {
        running = true;
        thread = new std::thread([this]() { this->startInternalProcess(); });
    }
}

void Thread::stop()
{
    stop_asked = true;
    condPause.notify_one(); // notify to continue
}

void Thread::quit()
{
    stop_asked = true;
    condPause.notify_one(); // notify to continue
}

void Thread::terminate()
{
    if (thread)
        delete thread;
    thread = nullptr;
    running = false;
}

void Thread::startInternalProcess()
{
    do {
        running = true;
        stop_asked = false;
        stopAtNextFinish = false;

        run();

        running = false;
        if (callbackFunction) callbackFunction();

        if (!stop_asked && !stopAtNextFinish) {
            condPause.wait(lockPause); // wait for a signal to continue
        }
    } while (!stop_asked && !stopAtNextFinish);
}

void Thread::wait(int time)
{
    int timeout = getCurrentMsSinceLocal() + time;
    int sleep_time = time > 10 ? 10 : time;
    while (isRunning() && getCurrentMsSinceLocal() < timeout) {
        sleepMs(sleep_time);
    }
}
