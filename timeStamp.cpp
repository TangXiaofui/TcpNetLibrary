/*
 * timeStamp.cpp
 *
 *  Created on: Jul 6, 2017
 *      Author: txh
 */

#include "timeStamp.h"
#include <sys/time.h>


TimeStamp::TimeStamp()
:microSecondsSinceEpoch_(0)
{

}

TimeStamp::TimeStamp(int64_t microSecondsSinceEpoch)
:microSecondsSinceEpoch_(microSecondsSinceEpoch)
{

}

int64_t TimeStamp::microSecondsSinceEpoch()
{
  return microSecondsSinceEpoch_;
}

TimeStamp TimeStamp::now()
{
  struct timeval tv;
  gettimeofday(&tv,nullptr);
  int64_t seconds = tv.tv_sec;
  return TimeStamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
}

std::string TimeStamp::toString() const
{
  char buf[32] = {0};
  int64_t seconds = microSecondsSinceEpoch_ / kMicroSecondsPerSecond;
  int64_t microseconds = microSecondsSinceEpoch_ % kMicroSecondsPerSecond;
  snprintf(buf, sizeof(buf)-1, "%ld .%06ld ", seconds, microseconds);
  return buf;
}

bool TimeStamp::valid()
{
  return microSecondsSinceEpoch_ > 0;
}

TimeStamp TimeStamp::invaild()
{
  return TimeStamp();
}

