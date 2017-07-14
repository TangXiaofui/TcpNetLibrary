/*
 * epoller.h
 *
 *  Created on: Jul 13, 2017
 *      Author: txh
 */

#ifndef EPOLLER_H_
#define EPOLLER_H_


#include <map>
#include <vector>
#include "utility.h"
#include "timeStamp.h"
#include "eventLoop.h"


class EventLoop;
class Channel;
struct epoll_event;


class Epoller : public noncopyable{
public:
  using ChannelList = std::vector<Channel*>;
  using EventList = std::vector<struct epoll_event>;
  using ChannelMap =  std::map<int,Channel*>;

  Epoller(EventLoop *loop);
  ~Epoller();
  TimeStamp poll(int timeoutMs,ChannelList* activeChannels);

  void updateChannel(Channel* channel);
  void removeChannel(Channel* channel);
  void assertInLoopThread();
private:
  static const int kInitEventListSize = 16;
  void fillActiveChannels(int numEvents,ChannelList *activeChannels) const;
  void update(int operation,Channel *channel);

  EventLoop *loop_;
  int epollfd_;
  EventList events_;
  ChannelMap channels_;

};

#endif /* EPOLLER_H_ */
