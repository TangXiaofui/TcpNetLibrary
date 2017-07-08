/*
 * eventLoopThreadPool.cpp
 *
 *  Created on: Jul 8, 2017
 *      Author: txh
 */


#include "eventLoopThreadPool.h"
#include "eventLoop.h"
#include "eventLoopThread.h"
#include <assert.h>

EventLoopThreadPool::EventLoopThreadPool(EventLoop *loop)
:baseloop_(loop),
 started_(0),
 numThreads_(0),
 next_(0)
{

}

EventLoopThreadPool::~EventLoopThreadPool()
{

}

void EventLoopThreadPool::setThreadNum(int numThreads)
{
  numThreads_ = numThreads;
}

void EventLoopThreadPool::start()
{
  assert(!started_);
  baseloop_->assertInLoopThread();
  started_ = true;
  for(int i = 0 ; i < numThreads_ ; i++)
    {
      EventLoopThread *t = new EventLoopThread;
      threads_.emplace_back(t);
      loops_.emplace_back(t->startLoop());
    }
}

EventLoop* EventLoopThreadPool::getNextLoop()
{
  baseloop_->assertInLoopThread();
  EventLoop *loop = baseloop_;

  if(!loops_.empty())
    {
      loop = loops_[next_];
      ++next_;
      if(static_cast<size_t>(next_) >= loops_.size())
	{
	  next_ = 0;
	}
    }
  return loop;
}

