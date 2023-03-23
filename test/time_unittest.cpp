#include<gtest/gtest.h>
#include "../base/time.h"


/// 测试默认构造下的各个功能
TEST(TimeStampTest, TimeStampBasic)
{
    TimeStamp t;
    EXPECT_EQ(t.getMillisecondsSinceEpoch(), 0);
    EXPECT_EQ(t.getMicrosecondsSinceEpoch(), 0);
    EXPECT_EQ(t.getSecondsSinceEpoch(), 0);
    std::string strTime = t.toFormatString(true);
    std::cout << strTime << std::endl;
    std::string strTime2 = t.toFormatString(false);
    std::cout << strTime2 << std::endl;
    struct timeval tm_val = t.toTimeval();
    EXPECT_EQ(tm_val.tv_sec, 0);
    EXPECT_EQ(tm_val.tv_usec, 0);
}

/// 测试now下的各个功能
TEST(TimeStampTest, TimeStampNow)
{
    TimeStamp t = TimeStamp::now();
    EXPECT_GT(t.getMillisecondsSinceEpoch(), 0);
    EXPECT_GT(t.getMicrosecondsSinceEpoch(), 0);
    EXPECT_GT(t.getSecondsSinceEpoch(), 0);
    std::string strTime = t.toFormatString(true);
    std::cout << strTime << std::endl;
    std::string toStrTime = TimeStamp::microsecondsFormat(t.getMicrosecondsSinceEpoch());
    EXPECT_EQ(strTime, toStrTime);
    std::string strTime2 = t.toFormatString(false);
    std::cout << strTime2 << std::endl;
    std::string toStrTime2 = TimeStamp::secondsFormat(t.getSecondsSinceEpoch());
    struct timeval tm_val = t.toTimeval();
    EXPECT_EQ(tm_val.tv_sec, t.getSecondsSinceEpoch());
    EXPECT_EQ(tm_val.tv_usec, t.getMicrosecondsSinceEpoch());
    
}