/*
 * timerQueue.h
 *
 *  Created on: Jul 6, 2017
 *      Author: txh
 */

#ifndef TIMERQUEUE_H_
#define TIMERQUEUE_H_

#include "utility.h"
#include "timerId.h"
#include "timer.h"
#include "timeStamp.h"
#include "channel.h"
#include <set>
#include <memory>
#include <vector>

class EventLoop;

class TimerQueue :public noncopyable{
public:
  TimerQueue(EventLoop *loop);
  ~TimerQueue();

  TimerId addTimer(const TimerCallBack&cb ,TimeStamp when,double interval);
  void addTimerInloop(Timer *timer);
  void cancelInLoop(TimerId timerId);
  void cancel(TimerId);

private:
  using Entry = std::pair<TimeStamp,Timer*>;
  using TimerList = std::set<Entry>;
  using ActiveTimer = std::pair<Timer*,int64_t>;
  using ActiveTimerSet = std::set<ActiveTimer>;

  void handleRead(TimeStamp receiveTime);
  std::vector<Entry> getExpired(TimeStamp now);
  void reset(const std::vector<Entry>& expire,TimeStamp now);

  bool insert(Timer* timer);

  EventLoop *loop_;
  const int timerfd_;
  Channel channel_;
  TimerList timers_;

  bool callingExpiredTimers_;

  ActiveTimerSet activeTiemrs_;
  ActiveTimerSet cancelingTimers_;
};



#endif /* TIMERQUEUE_H_ */
