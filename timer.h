/*
 * timer.h
 *
 *  Created on: Jul 6, 2017
 *      Author: txh
 */

#ifndef TIMER_H_
#define TIMER_H_

#include <functional>
#include "timerId.h"
#include "utility.h"
#include "timeStamp.h"
#include <atomic>

class Timer: public noncopyable{
public:
  Timer(const TimerCallBack& cb,TimeStamp when,double interval);
  void run();

  TimeStamp expiration();
  bool repeat();
  void restart(TimeStamp now);

  static int64_t numCreated();


private:
  const TimerCallBack timerCallBack_;
  TimeStamp expiration_;
  const double interval_;
  const bool repeat_;
  const int64_t sequence_;

  static std::atomic<int64_t> TimerCreated_;
};



#endif /* TIMER_H_ */
