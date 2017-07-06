/*
 * eventLoop.cpp
 *
 *  Created on: Jul 5, 2017
 *      Author: txh
 */
#include "eventLoop.h"
#include "assert.h"
#include "logging.h"
#include <poll.h>

__thread EventLoop* loopInThread_ = nullptr;
EventLoop::EventLoop():
threadId_(CurrentThread::tid()),
looping_(false),
pollfds_(new Poller(this)),
timerQueue_(new TimerQueue(this))
{
  trace("EventLoop %x create in thread %x",this,threadId_);
  //防止一个线程多次启动一个eventloop
  if(loopInThread_)
    {
      fatal("this thread has an eventloop");
    }
  else
    {
      loopInThread_ = this;
    }
}

EventLoop::~EventLoop()
{
  assert(!looping_);
  loopInThread_ = nullptr;
}


void EventLoop::loop()
{
  assert(!looping_);
  assertInLoopThread();
  looping_ = true;
  quit_ = false;

  while(!quit_){
      activeChannels_.clear();
      pollReturnTime_= pollfds_->poll(kPollTimeMs,&activeChannels_);
      for(ChannelList::iterator it = activeChannels_.begin(); it != activeChannels_.end();it++){
	  (*it)->handleEvent();
      }

  }

  trace("EventLoop %x stop loop",this);
  looping_ = false;
}

TimerId EventLoop::runAt(const TimeStamp& time,const TimerCallBack &cb)
{
  return timerQueue_->addTimer(cb,time,0.0f);
}
TimerId EventLoop::runAfter(double delay, const TimerCallBack &cb)
{
  return runAt(addTime(TimeStamp::now(),delay),cb);
}
TimerId EventLoop::runEvery(double interval,const TimerCallBack &cb)
{
  return timerQueue_->addTimer(cb,addTime(TimeStamp::now(),interval),interval);
}


void EventLoop::updateChannel(Channel* channel)
{
  assert(channel->ownerLoop() == this);
  assertInLoopThread();
  pollfds_->updateChannel(channel);
}

void EventLoop::quit()
{
  quit_ = true;
}

void EventLoop::assertInLoopThread()
{
  if(!isInLoopThread())
    {
      abortNotInLoopThread();
    }
}
bool EventLoop::isInLoopThread()
{
  //判断是否是其他线程
  return threadId_ == CurrentThread::tid();
}


void EventLoop::abortNotInLoopThread()
{
  fatal("eventloop %x is created in thread %x ,this thread is %x",this,threadId_,CurrentThread::tid());
}
