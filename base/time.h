#pragma once

#include <chrono>
#include <string>

class TimeStamp
{
public:
    static const int MillisecondsPerSecond = 1000;
    static const int MicrosecondsPerSecond = 1000 * 1000;
    static const int NanosecondsPerSecond = 1000 * 1000 * 1000;
    TimeStamp(uint64_t t = 0):timestamp_(t){}
    ~TimeStamp(){}
    static TimeStamp now();
    uint64_t getMillisecondsSinceEpoch(){return timestamp_ / MicrosecondsPerSecond;}
    uint64_t getMicrosecondsSinceEpoch(){return timestamp_ / MillisecondsPerSecond;}
    uint64_t getSecondsSinceEpoch() {return timestamp_ / NanosecondsPerSecond;}
    std::string toFormatString(bool showMicroseconds);
    static std::string microsecondsFormat(uint64_t timestamp);
    static std::string secondsFormat(uint64_t timestamp);

    struct timeval toTimeval();
private:
    uint64_t timestamp_;
};