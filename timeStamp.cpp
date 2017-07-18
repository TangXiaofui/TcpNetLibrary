/*
 * timeStamp.cpp
 *
 *  Created on: Jul 6, 2017
 *      Author: txh
 */

#include "timeStamp.h"
#include <sys/time.h>
#include <inttypes.h>

#ifndef __STDC_FORMAT_MACROS
#define __STDC_FORMAT_MACROS
#endif

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
  snprintf(buf, sizeof(buf)-1, "%" PRId64 ".%06" PRId64 "", seconds, microseconds);

  return buf;
}

std::string TimeStamp::toFormattedString(bool showMicroseconds) const
{
  char buf[32] = {0};
  time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
  struct tm tm_time;
  gmtime_r(&seconds, &tm_time);

  if (showMicroseconds)
  {
    int microseconds = static_cast<int>(microSecondsSinceEpoch_ % kMicroSecondsPerSecond);
    snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d.%06d",
             tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
             tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
             microseconds);
  }
  else
  {
    snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d",
             tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
             tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
  }
  return buf;
}


void TimeStamp::swap(TimeStamp & that)
{
  std::swap(microSecondsSinceEpoch_,that.microSecondsSinceEpoch_);
}

bool TimeStamp::valid()
{
  return microSecondsSinceEpoch_ > 0;
}

TimeStamp TimeStamp::invaild()
{
  return TimeStamp();
}

