#include "gtest/gtest.h"
#include "../base/AsyncLog.h"

TEST(AsyncLogTest, InitTest)
{
    EXPECT_EQ(true, AsyncLog::init("InitTest.log"));
    ASSERT_EQ(false, AsyncLog::init("InitTest2.log"));
    ASSERT_EQ(true, AsyncLog::uninit());
}

TEST(AsyncLogTest, LogNoFatal)
{
    EXPECT_EQ(true, AsyncLog::init("LogNoFatal.log"));
    AsyncLog::setLevel(LOG_LEVEL_DEBUG);
    ASSERT_EQ(true, AsyncLog::isRunning());
    for(int i = 0; i < 5; ++i)
    {
        LOGD("In TEST LOGNOFATAL:");
        LOGI("In TEST LOGNOFATAL:");
        LOGW("In TEST LOGNOFATAL:");
        LOGE("In TEST LOGNOFATAL:");
        LOGC("In TEST LOGNOFATAL:");
    }
    EXPECT_EQ(true, AsyncLog::uninit());
}

void logFunc()
{
    LOGD("In TEST LOGNOFATAL:");
    LOGI("In TEST LOGNOFATAL:");
    LOGW("In TEST LOGNOFATAL:");
    LOGF("FATAL ERROR!!!");
}

// TEST(AsyncLogTest, LogWithFatal)
// {
//     EXPECT_EQ(true, AsyncLog::init("LogWithFatal.log"));
//     AsyncLog::setLevel(LOG_LEVEL_DEBUG);
//     ASSERT_EQ(true, AsyncLog::isRunning());
//     logFunc();
//     EXPECT_EQ(false, AsyncLog::isRunning());
// }