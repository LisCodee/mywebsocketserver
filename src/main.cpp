#include <iostream>
#include <string>
#include <gtest/gtest.h>
#include "../base/AsyncLog.h"


int main()
{
    AsyncLog::init("mainLog.log");
    LOGE("error");
    AsyncLog::uninit();
    return 0;
}