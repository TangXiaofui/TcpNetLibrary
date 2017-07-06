/*
 * eventLoop.h
 *
 *  Created on: Jul 5, 2017
 *      Author: txh
 */

#ifndef EVENTLOOP_H_
#define EVENTLOOP_H_

#include "utility.h"
#include "channel.h"
#include "poller.h"
#include "timerQueue.h"
#include <thread>
#include <memory>
#include <mutex>

class EventLoop:public noncopyable{
public:
  using ChannelList = std::vector<Channel*>;
  EventLoop();
  ~EventLoop();

  void loop();
  void updateChannel(Channel*);
  void quit();

  void runInLoop(const Functor& cb);
  void queueInloop(const Functor& cb);

  void wakeup();


  TimerId runAt(const TimeStamp& time,const TimerCallBack &cb);
  TimerId runAfter(double delay, const TimerCallBack &cb);
  TimerId runEvery(double interval,const TimerCallBack &cb);

  void assertInLoopThread();
  bool isInLoopThread();

private:
  void abortNotInLoopThread();
  void handleRead();
  void doPendingFunctors();


  const long threadId_;
  const int kPollTimeMs = 10000;
  bool looping_;
  bool quit_;
  bool callingPendingFunctors_;

  std::shared_ptr<Poller> pollfds_;
  ChannelList activeChannels_;
  TimeStamp pollReturnTime_;
  std::shared_ptr<TimerQueue> timerQueue_;//必须放到pollfd后面

  int wakeupFd_;
  std::shared_ptr<Channel> wakeupChannel_;
  std::mutex mtx_;
  std::vector<Functor> pendingFunctor_;
};



#endif /* EVENTLOOP_H_ */
