/*
 * eventLoopThread.h
 *
 *  Created on: Jul 6, 2017
 *      Author: txh
 */

#ifndef EVENTLOOPTHREAD_H_
#define EVENTLOOPTHREAD_H_


#include "utility.h"
#include <thread>
#include <mutex>
#include <condition_variable>

class EventLoop;

class EventLoopThread:public noncopyable{
public:
  EventLoopThread();
  ~EventLoopThread();

  EventLoop* startLoop();

private:
  void threadFunc();
  EventLoop* loop_;

  std::thread thread_;
  std::mutex mtx_;
  std::condition_variable cond_;
};


#endif /* EVENTLOOPTHREAD_H_ */
