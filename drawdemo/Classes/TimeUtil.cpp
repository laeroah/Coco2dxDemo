//
//  TimeUtil.cpp
//  drawdemo
//
//  Created by Hao Wang on 14-1-24.
//
//

#include "TimeUtil.h"
#include <sys/time.h>

unsigned long long  TimeUtil::getCurrentTime()
{
    struct timeval tv;
    gettimeofday(&tv,NULL);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}