/*
 * timerQueue.cpp
 *
 *  Created on: Jul 6, 2017
 *      Author: txh
 */



#include "timerQueue.h"
#include "logging.h"
#include "eventLoop.h"
#include <sys/timerfd.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>


int createTimerfd()
{
  int fd = ::timerfd_create(CLOCK_MONOTONIC,TFD_CLOEXEC | TFD_NONBLOCK);
  if(fd < 0)
    {
      log_error("create timerfd failed\n");
    }
  return fd;
}

struct timespec howMuchTimeFromNow(TimeStamp when)
{
  int64_t microseconds = when.microSecondsSinceEpoch()
			  - TimeStamp::now().microSecondsSinceEpoch();
  if(microseconds < 100)
    microseconds = 100;

  struct timespec ts;
  ts.tv_sec = static_cast<time_t>(microseconds / TimeStamp::kMicroSecondsPerSecond);
  ts.tv_nsec = static_cast<time_t>((microseconds % TimeStamp::kMicroSecondsPerSecond)*1000);
  return ts;
}

//更新超时时间，当有更早的超时事件插入队列时调用
void resetTimerfd(int timerfd,TimeStamp expiration)
{
  struct itimerspec newValue;
  struct itimerspec oldValue;
  ::bzero(&newValue,sizeof newValue);
  ::bzero(&oldValue,sizeof oldValue);
  newValue.it_value = howMuchTimeFromNow(expiration);
  int ret = ::timerfd_settime(timerfd,0,&newValue,&oldValue);
  if(ret < 0)
    log_error("timerfd_settime failed");
}

//清除标志
void readTimerfd(int timerfd,TimeStamp now)
{
  uint64_t howmany;
  ssize_t n = ::read(timerfd,&howmany,sizeof(howmany));
  log_trace("TimerQueue::readTimerfd %ld at %s\n",howmany,now.toString().c_str());
  if( n != sizeof howmany)
    {
      log_error("TimerQueue::readTimerfd n = %d\n",n);
    }
}

TimerQueue::TimerQueue(EventLoop *loop)
:loop_(loop),
timerfd_(createTimerfd()),
channel_(loop,timerfd_),
timers_()
{
  channel_.setReadCallBack(
      std::bind(&TimerQueue::handleRead,this,std::placeholders::_1));
  channel_.enableReading();
}

TimerQueue::~TimerQueue()
{
  ::close(timerfd_);
  for(TimerList::iterator it = timers_.begin() ; it != timers_.end() ; it++)
    {
      delete it->second;
    }
}

//划分成下面两个函数的目的是为了支持其他线程调用该函数，保证线程安全
TimerId TimerQueue::addTimer(const TimerCallBack&cb ,TimeStamp when,double interval)
{
  Timer *timer = new Timer(cb,when,interval);
  loop_->runInLoop(std::bind(&TimerQueue::addTimerInloop,this,timer));
  return TimerId(timer);
}

void TimerQueue::addTimerInloop(Timer *timer)
{
  loop_->assertInLoopThread();
  bool earliestChange = insert(timer);
  if(earliestChange)
    {
      resetTimerfd(timerfd_,timer->expiration());
    }
}

void TimerQueue::cancel()
{

}


void TimerQueue::handleRead(TimeStamp receiveTime)
{
  loop_->assertInLoopThread();
  TimeStamp now(TimeStamp::now());
  readTimerfd(timerfd_,now);

  std::vector<Entry> expire = getExpired(now);
  for(vector<Entry>::iterator it = expire.begin(); it != expire.end(); it++)
    {
      it->second->run();
    }
  reset(expire,now);
}

std::vector<TimerQueue::Entry> TimerQueue::getExpired(TimeStamp now)
{
  std::vector<Entry> expired;
  Entry sentry = std::make_pair(now,reinterpret_cast<Timer*>(UINTPTR_MAX));
  TimerList::iterator it = timers_.lower_bound(sentry);
  assert(it == timers_.end() || now < it->first);
  std::copy(timers_.begin(),it,std::back_inserter(expired));
  //将超时的任务从队列中移除
  timers_.erase(timers_.begin(),it);
  return expired;
}

void TimerQueue::reset(const std::vector<Entry>& expire,TimeStamp now)
{
  for(std::vector<Entry>::const_iterator it = expire.begin(); it != expire.end(); it++)
    {
      if(it->second->repeat())
	{
	  it->second->restart(now);
	  insert(it->second);
	}
      else
	{
	  delete it->second;
	}
    }

    TimeStamp nextExpire;
    if(timers_.size())
      {
	nextExpire = timers_.begin()->first;
      }
    if(nextExpire.valid())
      resetTimerfd(timerfd_,nextExpire);

}

bool TimerQueue::insert(Timer* timer)
{
  bool earliestChanged = false;
  TimeStamp when = timer->expiration();
  TimerList::const_iterator it = timers_.begin();
  if(it == timers_.end() || when < it->first)
    {
      //当超时时间比原来的小，则需要更新超时时间
      earliestChanged = true;
    }

  std::pair<TimerList::iterator,bool> result =
      timers_.emplace(std::make_pair(when,timer));
  assert(result.second);
  return earliestChanged;

}
