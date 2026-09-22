#ifndef TIMER_H
#define TIMER_H
#include <chrono>
#include <cstdint>


class Timer{
private:
    std::chrono::steady_clock::time_point mStart_time;

public:
    using Nanos = std::chrono::nanoseconds;
    using Micros = std::chrono::microseconds;
    using Millis = std::chrono::milliseconds;
    using Seconds = std::chrono::seconds;
    using Minutes = std::chrono::minutes;
    using Hours = std::chrono::hours;
    Timer()
    {
        mStart_time = std::chrono::steady_clock::now();
    }
    void restart(){
        mStart_time = std::chrono::steady_clock::now();
    }
    template <typename T>
    uint64_t glance() const{
        auto now = std::chrono::steady_clock::now();
        return static_cast<uint64_t>(std::chrono::duration_cast<T>(now-mStart_time).count());
    }
    template <typename T>
    uint64_t click(){
        auto now = std::chrono::steady_clock::now();
        auto elapsed = std::chrono::duration_cast<T>(now-mStart_time).count();
        mStart_time = now;
        return static_cast<uint64_t>(elapsed);
    }

};

#endif  // TIMER_H
