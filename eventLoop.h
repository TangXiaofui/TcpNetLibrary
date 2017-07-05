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
#include <thread>
#include <memory>


class EventLoop:public noncopyable{
public:
  using ChannelList = std::vector<Channel*>;
  EventLoop();
  ~EventLoop();

  void loop();
  void updateChannel(Channel*);
  void quit();


  void assertInLoopThread();
  bool isInLoopThread();

private:
  void abortNotInLoopThread();
  const long threadId_;
  const int kPollTimeMs = 10000;
  bool looping_;
  bool quit_;
  ChannelList activeChannels_;

  std::shared_ptr<Poller> pollfds_;
};



#endif /* EVENTLOOP_H_ */
