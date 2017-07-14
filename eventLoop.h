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
#ifndef USE_EPOLLER
#include "poller.h"
#else
#include "epoller.h"
#endif
#include "timerQueue.h"
#include <thread>
#include <memory>
#include <mutex>

class EventLoop:public noncopyable{
public:
  //所有using最后封装在一个头文件
  using ChannelList = std::vector<Channel*>;
  EventLoop();
  ~EventLoop();

  void loop();

  //往poller注册channel
  void updateChannel(Channel*channel);
  void removeChannel(Channel* channel);
  void quit();

  //用于事件分发，同一个线程同步调用，其他线程则放入vector等待事件处理后调用
  void runInLoop(const Functor& cb);

  //放入vector等待调用
  void queueInloop(const Functor& cb);

  //唤醒等待事件发生的pollfd
  void wakeup();


  TimerId runAt(const TimeStamp& time,const TimerCallBack &cb);
  TimerId runAfter(double delay, const TimerCallBack &cb);
  TimerId runEvery(double interval,const TimerCallBack &cb);

  void cancel(TimerId timerId);

  void assertInLoopThread();
  bool isInLoopThread();

private:
  void abortNotInLoopThread();

  //用于处理eventfd
  void handleRead();

  //当处理完监听时间后，执行用户任务
  void doPendingFunctors();


  const long threadId_;
  const int kPollTimeMs = 10000;
  bool looping_;
  bool quit_;
  bool callingPendingFunctors_;

#ifndef USE_EPOLLER
  std::shared_ptr<Poller> pollfds_;
#else
  std::shared_ptr<Epoller> pollfds_;
#endif
  ChannelList activeChannels_;
  TimeStamp pollReturnTime_;
  std::shared_ptr<TimerQueue> timerQueue_;//必须放到pollfd后面

  int wakeupFd_;
  std::shared_ptr<Channel> wakeupChannel_;
  std::mutex mtx_;
  std::vector<Functor> pendingFunctor_;
};



#endif /* EVENTLOOP_H_ */
