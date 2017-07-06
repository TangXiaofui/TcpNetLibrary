/*
 * timer.cpp
 *
 *  Created on: Jul 6, 2017
 *      Author: txh
 */

#include "timer.h"



std::atomic<int64_t> Timer::TimerCreated_(0);

Timer::Timer(const TimerCallBack& cb,TimeStamp when,double interval)
:timerCallBack_(cb),
 expiration_(when),
 interval_(interval),
 repeat_(interval > 0.0),
 sequence_(TimerCreated_++)
{

}
void  Timer::run()
{
  timerCallBack_();
}

TimeStamp  Timer::expiration()
{
  return expiration_;
}

int64_t  Timer::numCreated()
{
  return TimerCreated_;
}

bool Timer::repeat()
{
  return repeat_;
}

void Timer::restart(TimeStamp now)
{
  if(repeat_)
    {
      expiration_ = addTime(now,interval_);
    }
  else
    {
      expiration_ = TimeStamp::invaild();
    }
}

