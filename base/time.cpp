#include "time.h"
#include <chrono>

TimeStamp TimeStamp::now()
{
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    TimeStamp t(now.time_since_epoch().count());
    return t;
}

std::string TimeStamp::toFormatString(bool showMicroseconds)
{
    if(showMicroseconds)
        return microsecondsFormat(this->getMicrosecondsSinceEpoch());
    return secondsFormat(this->getSecondsSinceEpoch());
}

std::string TimeStamp::microsecondsFormat(uint64_t microsecond)
{
    time_t seconds = static_cast<time_t>(microsecond / MicrosecondsPerSecond);
    struct tm tm_time;
#ifdef WIN32
    localtime_s(&tm_time, &seconds);
#else
    localtime_r(&seconds, &tm_time);
#endif
    char szTime[32]{0};
    int microseconds = microsecond % MicrosecondsPerSecond;
    snprintf(szTime, sizeof(szTime), "%4d-%02d-%02d %02d:%02d:%02d.%06d", tm_time.tm_year + 1900,
             tm_time.tm_mon + 1, tm_time.tm_mday, tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec, microseconds);
    return szTime;
}

std::string TimeStamp::secondsFormat(uint64_t timestamp)
{
    time_t seconds = static_cast<time_t>(timestamp);
    struct tm tm_time;
#ifdef WIN32
    localtime_s(&tm_time, &seconds);
#else
    localtime_r(&seconds, &tm_time);
#endif
    char szTime[32]{0};
    snprintf(szTime, sizeof(szTime), "%4d-%02d-%02d %02d:%02d:%02d", tm_time.tm_year + 1900,
             tm_time.tm_mon + 1, tm_time.tm_mday, tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
    return szTime;
}

timeval TimeStamp::toTimeval()
{
    struct timeval tm_val
    {
        timestamp_ / NanosecondsPerSecond, timestamp_ / MillisecondsPerSecond
    };
    return tm_val;
}