/*
 * eventLoopThreadPool.h
 *
 *  Created on: Jul 8, 2017
 *      Author: txh
 */

#ifndef EVENTLOOPTHREADPOOL_H_
#define EVENTLOOPTHREADPOOL_H_

#include "utility.h"
#include <vector>
#include <memory>

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool:public noncopyable{
public:
  EventLoopThreadPool(EventLoop *loop);
  ~EventLoopThreadPool();

  void setThreadNum(int numThreads);
  void start();
  EventLoop* getNextLoop();

private:
  EventLoop *baseloop_;
  bool started_;
  int numThreads_;
  int next_;
  std::vector<EventLoopThread*> threads_;
  std::vector<EventLoop*> loops_;

};


#endif /* EVENTLOOPTHREADPOOL_H_ */
