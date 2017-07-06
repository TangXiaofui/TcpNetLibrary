/*
 * eventLoopThread.cpp
 *
 *  Created on: Jul 6, 2017
 *      Author: txh
 */


#include "eventLoopThread.h"
#include "eventLoop.h"


EventLoopThread::EventLoopThread()
:loop_(nullptr),
 mtx_()
{

}

EventLoopThread::~EventLoopThread()
{
  loop_->quit();
  if(thread_.joinable())
    thread_.join();
}

EventLoop* EventLoopThread::startLoop()
{
  thread_ = std::move(std::thread(std::bind(&EventLoopThread::threadFunc,this)));
  {
     std::unique_lock<std::mutex> lock(mtx_);
     while(loop_ == nullptr)
       cond_.wait(lock);
  }
  return loop_;
}


void EventLoopThread::threadFunc()
{
  EventLoop loop;
  {
    std::unique_lock<std::mutex> lock(mtx_);
    loop_ = &loop;
    cond_.notify_one();
  }
  loop.loop();
}

