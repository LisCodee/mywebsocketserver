#include "AsyncLog.h"
#include <cstring>
#include <stdarg.h>
#include <iostream>

#ifdef WIN32
#include <time.h>
#else
#include <sys/timeb.h>
#include <sys/syscall.h>
#include <unistd.h>
#endif

//debug 宏变量，向控制台输出日志
#define LOGSTD

// 初始化静态变量
bool AsyncLog::bTruncateLongLog_ = true;
uint32_t AsyncLog::uiTruncateSize_ = 256;
bool AsyncLog::bToFile_ = true;
std::string AsyncLog::strFileName_ = "";
std::string AsyncLog::strPid_ = "";
LOG_LEVEL AsyncLog::euCurrentLevel_ = LOG_LEVEL_INFO;
uint32_t AsyncLog::uiCurrentWritten_ = 0;
FILE *AsyncLog::hLogFile_ = 0;
std::queue<std::string> AsyncLog::listLinesToWrite_;
std::vector<std::shared_ptr<std::thread>> AsyncLog::listWriteThread_;
std::mutex AsyncLog::mutexWrite_;
std::condition_variable AsyncLog::cvWrite_;
bool AsyncLog::bExit_ = false;
bool AsyncLog::bRunning_ = false;
uint8_t AsyncLog::u8WrtieThreads_ = 2;

std::string generateLogFileName(const std::string &pid)
{
    char szNow[64];
    time_t now = time(NULL);
    tm time;
#ifdef WIN32
    localtime_s(&time, &now);
#else
    localtime_r(&now, &time);
#endif
    strftime(szNow, sizeof szNow, "%Y%m%d%H%M%S", &time);
    std::string fileName;
    fileName += szNow;
    fileName += ".";
    fileName += pid;
    fileName += ".log";
    return fileName;
}

bool AsyncLog::init(const char *pszLogFileName, bool bToFile, bool bTruncate, uint32_t iMaxSize)
{
    if (bRunning_)
        return false;
    // 获取进程pid
    char szPid[8];
#ifdef WIN32
    snprintf(szPid, sizeof szPid, "%06d", (int)::GetCurrentProcessId());
#else
    snprintf(szPid, sizeof szPid, "%06d", (int)::getpid());
#endif
    strPid_ = szPid;
    bToFile_ = bToFile;
    bTruncate = bTruncate;
    uiTruncateSize_ = iMaxSize > 64 ? iMaxSize : 64; // 默认最小长度64
    // 文件名为空，创建默认文件
    if ((pszLogFileName == 0 || strlen(pszLogFileName) == 0) && bToFile_)
    {
        strFileName_.clear();
        strFileName_ = generateLogFileName(strPid_);
    }
    else
        strFileName_ = pszLogFileName;
    // 打开文件句柄
    if (bToFile_)
    {
        if (hLogFile_)
            fclose(hLogFile_);
        hLogFile_ = fopen(pszLogFileName, "w+");

        if (hLogFile_ == nullptr)
            return false;
    }
    // 创建写日志线程
    for (uint8_t i = 0; i < u8WrtieThreads_; ++i)
    {
        listWriteThread_.push_back(std::shared_ptr<std::thread>(new std::thread(writeThreadProc)));
    }
    bRunning_ = true;
    return true;
}

bool AsyncLog::uninit()
{
    bExit_ = true;
    cvWrite_.notify_all();
    for (auto sptrThread : listWriteThread_)
    {
        if (sptrThread.get()->joinable())
        {
            sptrThread.get()->join();
        }
    }

    if (hLogFile_ != 0)
        fclose(hLogFile_);
    hLogFile_ = 0;
    bRunning_ = false;
    return true;
}

bool AsyncLog::setLevel(LOG_LEVEL level)
{
    euCurrentLevel_ = level;
    return true;
}

bool AsyncLog::isRunning()
{
    return bRunning_;
}

bool AsyncLog::output(LOG_LEVEL nLevel, const char *pszFmt, ...)
{
    if (nLevel != LOG_LEVEL_CRITICAL)
    {
        if (nLevel < euCurrentLevel_)
            return false;
    }
    std::string logLine;
    makeLinePrefix(nLevel, logLine);
    // log正文
    char *pszLogContent = new char[uiTruncateSize_];
    va_list ap;
    va_start(ap, pszFmt);
    int nContentLen = vsnprintf(pszLogContent, uiTruncateSize_, pszFmt, ap);
    va_end(ap);

    // std::cout << pszLogContent;
    logLine += pszLogContent;
    logLine += "\n";

    if (nLevel == LOG_LEVEL_FATAL)
    {
        if (bToFile_)
            writeToFile(logLine);
        else
            std::cout << logLine;
        crash();
    }

    // 将日志内容加入待写入队列
    std::lock_guard<std::mutex> lock(mutexWrite_);
    listLinesToWrite_.push(std::move(logLine));
    cvWrite_.notify_all();
    return true;
}

bool AsyncLog::output(LOG_LEVEL nLevel, const char *pszFileName, int nLineNo, const char *pszFmt, ...)
{
    if (nLevel != LOG_LEVEL_CRITICAL)
    {
        if (nLevel < euCurrentLevel_)
            return false;
    }
    std::string logLine;
    makeLinePrefix(nLevel, logLine);

    // 函数签名
    char szFileName[512]{0};
    snprintf(szFileName, sizeof szFileName, "[%s:%d]", pszFileName, nLineNo);
    logLine += szFileName;
    // log正文
    char *pszLogContent = new char[uiTruncateSize_];
    va_list ap;
    va_start(ap, pszFmt);
    int nContentLen = vsnprintf(pszLogContent, uiTruncateSize_, pszFmt, ap);
    va_end(ap);

    // std::cout << pszLogContent;
    logLine += pszLogContent;
    logLine += "\n";

    if (nLevel == LOG_LEVEL_FATAL)
    {
#ifdef LOGSTD
        std::cout << logLine;
#endif
        if (bToFile_)
            writeToFile(logLine);
        else
            std::cout << logLine;
        crash();
    }

    // 将日志内容加入待写入队列
    std::lock_guard<std::mutex> lock(mutexWrite_);
    listLinesToWrite_.push(std::move(logLine));
    cvWrite_.notify_all();
    return true;
}

void AsyncLog::makeLinePrefix(LOG_LEVEL nLevel, std::string &strPrefix)
{
    strPrefix = "[INFO]";
    if (nLevel == LOG_LEVEL_DEBUG)
        strPrefix = "[DEBUG]";
    else if (nLevel == LOG_LEVEL_WARNING)
        strPrefix = "[WARN]";
    else if (nLevel == LOG_LEVEL_ERROR)
        strPrefix = "[ERROR]";
    else if (nLevel == LOG_LEVEL_FATAL)
        strPrefix = "[FATAL]";
    else if (nLevel == LOG_LEVEL_CRITICAL)
        strPrefix = "[CRITICAL]";

    char szNow[64]{0};
    getTime(szNow, sizeof szNow);

    strPrefix += "[";
    strPrefix += szNow;
    strPrefix += "]";
    // 当前线程信息
    char szThreadId[32]{0};
#ifdef WIN32
    DWORD threadId = ::GetCurrentThreadId();
#else
    int threadId = syscall(SYS_gettid);
#endif
    snprintf(szThreadId, sizeof szThreadId, "[%d]", (int)threadId);
    strPrefix += szThreadId;
}

void AsyncLog::getTime(char *pszTime, int nTimeStrLen)
{
    struct timeb tp;
    ftime(&tp);
    time_t now = tp.time;
    tm time;
#ifdef _WIN32
    localtime_s(&time, &now);
#else
    localtime_r(&now, &time);
#endif
    snprintf(pszTime, nTimeStrLen, "%04d-%02d-%02d %02d:%02d:%02d:%03d", time.tm_year + 1900, time.tm_mon + 1, time.tm_mday, time.tm_hour, time.tm_min, time.tm_sec, tp.millitm);
}

bool AsyncLog::createNewFile(const char *pszLogFileName)
{
    if (hLogFile_)
        fclose(hLogFile_);
    hLogFile_ = fopen(strFileName_.c_str(), "w+");
    return hLogFile_ != nullptr;
}

bool AsyncLog::writeToFile(const std::string &data)
{
    std::string dataLocal(data);
    int ret = 0;
    while (true)
    {
        ret = fwrite(dataLocal.c_str(), 1, dataLocal.size(), hLogFile_);
        if (ret < 0)
        {
            return false;
        }
        else if (ret <= dataLocal.size())
        {
            dataLocal.erase(0, ret);
        }
        if (dataLocal.empty())
            break;
    }
    fflush(hLogFile_);
    return true;
}

void AsyncLog::crash()
{
    int *p = nullptr;
    *p = 0;
}

void AsyncLog::writeThreadProc()
{
    bRunning_ = true;
    while (true)
    {
        std::string strLogLine;
        {
            std::unique_lock<std::mutex> guard(mutexWrite_);
            while (listLinesToWrite_.empty())
            {
                if (bExit_)
                    return;
                cvWrite_.wait(guard);
            }
            strLogLine = listLinesToWrite_.front();
            listLinesToWrite_.pop();
        }
#ifdef LOGSTD
        std::cout << strLogLine;
#endif
        if (bToFile_)
        {
            // std::unique_lock<std::mutex> guard(mutexWrite_);
            if ((strFileName_.empty() && hLogFile_ == 0) || uiCurrentWritten_ >= uiMaxFileSize_)
            {
                std::unique_lock<std::mutex> guard(mutexWrite_);
                strFileName_ = generateLogFileName(strPid_);
                if (!createNewFile(strFileName_.c_str()))
                {
                    crash();
                }
                uiCurrentWritten_ = 0;
            }
            writeToFile(strLogLine);
            uiCurrentWritten_ += strLogLine.size();
        }
        else
            std::cout << strLogLine << std::endl;
    }
}
