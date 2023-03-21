#include <iostream>
#include <string>
#include "../base/AsyncLog.h"


int main()
{
    AsyncLog::init("mainLog.log");
    LOGE("error");
    AsyncLog::uninit();
    return 0;
}