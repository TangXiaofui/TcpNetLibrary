/*
 * eventLoop.cpp
 *
 *  Created on: Jul 5, 2017
 *      Author: txh
 */
#include "eventLoop.h"
#include <assert.h>
#include "logging.h"
//#include <poll.h>
#include <sys/eventfd.h>
#include <unistd.h>


//尽力不用pipe，首先文件描述符多，并且缓冲区不确定
static int createEvent()
{
  int fd = ::eventfd(0,EFD_NONBLOCK| EFD_CLOEXEC);
  if(fd < 0)
    log_fatal("create eventfd failed");
  return fd;
}

//线程全局变量
__thread EventLoop* loopInThread_ = nullptr;

//注意：初始化列表的顺序只于定义的顺序有关
EventLoop::EventLoop():
threadId_(CurrentThread::tid()),
looping_(false),
#ifndef USE_EPOLLER
pollfds_(new Poller(this)),
#else
pollfds_(new Epoller(this)),
#endif
timerQueue_(new TimerQueue(this)),
wakeupFd_(createEvent()),
wakeupChannel_(new Channel(this,wakeupFd_))
{
  log_trace("EventLoop %x create in thread %x",this,threadId_);
  //防止一个线程多次启动一个eventloop
  if(loopInThread_)
    {
      log_fatal("this thread has an eventloop");
    }
  else
    {
      loopInThread_ = this;
    }
  wakeupChannel_->setReadCallBack(std::bind(&EventLoop::handleRead,this));
  wakeupChannel_->enableReading();
}

EventLoop::~EventLoop()
{
  assert(!looping_);
  loopInThread_ = nullptr;
  ::close(wakeupFd_);
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
	  (*it)->handleEvent(pollReturnTime_);
      }
      doPendingFunctors();
  }

  log_trace("EventLoop %x stop loop",this);
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

void EventLoop::cancel(TimerId timerId)
{
  timerQueue_->cancel(timerId);
}

void EventLoop::updateChannel(Channel* channel)
{
  assert(channel->ownerLoop() == this);
  assertInLoopThread();
  pollfds_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel)
{
  assert(channel->ownerLoop() == this);
  assertInLoopThread();
  pollfds_->removeChannel(channel);
}

void EventLoop::quit()
{
  quit_ = true;
  if(!isInLoopThread())
    {
      wakeup();
    }
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
  log_fatal("eventloop %x is created in thread %x ,this thread is %x",this,threadId_,CurrentThread::tid());
}


void EventLoop::runInLoop(const Functor& cb)
{
  if(isInLoopThread())
    {
      cb();
    }
  else
    {
      queueInloop(cb);
    }
}

void EventLoop::queueInloop(const Functor& cb)
{
  {
    std::unique_lock<mutex> lock(mtx_);
    pendingFunctor_.push_back(cb);
  }


  //情况二是为了保证在执行doPendingFunctors时，其他线程又调用了queueInloop
  if(!isInLoopThread() || callingPendingFunctors_)
    {
      wakeup();
    }
}

void EventLoop::wakeup()
{
  uint64_t one = 1;
  ssize_t n = ::write(wakeupFd_,&one,sizeof one);
  if(n != sizeof one)
    {
      log_error("EventLoop write eventfd failed");
    }
}

void EventLoop::handleRead()
{
  uint64_t one ;
  ssize_t n = ::read(wakeupFd_,&one,sizeof one);
    if(n != sizeof one)
      {
	log_error("EventLoop read eventfd failed");
      }
}

void EventLoop::doPendingFunctors()
{
  std::vector<Functor> functors;
  callingPendingFunctors_ = true;
  {
    std::unique_lock<mutex> lock(mtx_);
    functors.swap(pendingFunctor_);
  }
  for(size_t i = 0; i < functors.size(); i++)
    {
      functors[i]();
    }
  callingPendingFunctors_ = false;
}

