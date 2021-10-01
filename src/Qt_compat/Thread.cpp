#include "Thread.h"

Thread::Thread()
{
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
    if (thread != nullptr && running)
        delete thread;
    running = true;
    thread = new std::thread([this]() { this->startInternalProcess(); });
}

void Thread::join()
{
    if (thread) thread->join();
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
    running = true;
    stop_asked = false;

    run();

    running = false;
    if (callbackFunction) callbackFunction();
}

void Thread::wait(int time)
{
    int timeout = getCurrentMsSinceLocal() + time;
    while (isRunning() && getCurrentMsSinceLocal() < timeout) {
        sleepMs(10);
    }
}
