#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>

#define LOGD(...) AsyncLog::output(LOG_LEVEL_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOGI(...) AsyncLog::output(LOG_LEVEL_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define LOGW(...) AsyncLog::output(LOG_LEVEL_WARNING, __FILE__, __LINE__, __VA_ARGS__)
#define LOGE(...) AsyncLog::output(LOG_LEVEL_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define LOGF(...) AsyncLog::output(LOG_LEVEL_FATAL, __FILE__, __LINE__, __VA_ARGS__)
#define LOGC(...) AsyncLog::output(LOG_LEVEL_CRITICAL, __FILE__, __LINE__, __VA_ARGS__)

enum LOG_LEVEL
{
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL,
    LOG_LEVEL_CRITICAL
};

class AsyncLog
{
public:
    /// @brief 初始化异步写日志类
    /// @param pszLogFileName 日志文件名
    /// @param bToFile 是否输出到文件
    /// @param bTruncate 是否截断长日志
    /// @param iMaxSize 长日志标准
    /// @return
    static bool init(const char *pszLogFileName, bool bToFile = true, bool bTruncate = true, uint32_t iMaxSize = 256);
    static bool uninit();
    static bool setLevel(LOG_LEVEL level);
    static bool isRunning();

    /// @brief 不输出线程ID和所在函数签名、行号
    /// @param nLevel 日志等级
    /// @param pszFmt 出错信息
    /// @param  
    /// @return 
    static bool output(LOG_LEVEL nLevel, const char* pszFmt, ...);

    /// @brief 输出线程ID和所在函数签名、行号
    /// @param nLevel 日志等级
    /// @param pszFileName 出错代码所在文件
    /// @param nLineNo 出错代码所在行数
    /// @param pszFmt 出错信息
    /// @param  
    /// @return 
    static bool output(LOG_LEVEL nLevel, const char* pszFileName, int nLineNo,
    const char* pszFmt, ...);

private:
    AsyncLog() = delete;
    ~AsyncLog() = delete;
    AsyncLog(const AsyncLog &) = delete;
    AsyncLog &operator=(const AsyncLog &) = delete;

    /// @brief LOG前缀生成
    /// @param nLevel 日志等级
    /// @param strPrefix 生成后的前缀
    static void makeLinePrefix(LOG_LEVEL nLevel, std::string& strPrefix);

    /// @brief 获取当前日期时间
    /// @param pszTime 接受日期时间的char数组
    /// @param nTimeStrLen char数组大小
    static void getTime(char* pszTime, int nTimeStrLen);

    /// @brief 创建新的日志文件，并改变FILE句柄
    /// @param pszLogFileName 新日志文件名
    /// @return 是否成功
    static bool createNewFile(const char *pszLogFileName);

    /// @brief 将data写入FILE句柄指向的文件
    /// @param data 要写入的日志
    /// @return 
    static bool writeToFile(const std::string &data);

    /// @brief 使程序主动崩溃
    static void crash();
    static void writeThreadProc();

    // static std::string& processVaFmt()

private:
    static bool bTruncateLongLog_;                                     // 截断长日志
    static uint32_t uiTruncateSize_;                                   // 截断长度
    static bool bToFile_;                                              // 是否输出到文件
    static std::string strFileName_;                                   // 日志文件名
    static std::string strPid_;                                        // 进程id
    static LOG_LEVEL euCurrentLevel_;                                  // 当前日志等级
    static uint32_t uiMaxFileSize_;                                    // 单个日志文件最大size
    static uint32_t uiCurrentWritten_;                                 // 当前写入字节数
    static FILE *hLogFile_;                                            // 当前文件句柄
    static uint8_t u8WrtieThreads_;                                    // 写日志线程数
    static std::queue<std::string> listLinesToWrite_;                 // 待写入日志
    static std::vector<std::shared_ptr<std::thread>> listWriteThread_; // 写日志线程
    static std::mutex mutexWrite_;                                      // 保护listLinesToWrite_的互斥量
    static std::condition_variable cvWrite_;                           // 条件变量
    static bool bExit_;                                                // 退出标志位
    static bool bRunning_;                                             // 运行标志位
};