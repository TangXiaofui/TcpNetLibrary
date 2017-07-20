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

//需要判断epollhup 与 pollhup是否相同，否则channel的handleEent会出现错误


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

  //更新监听的时间列表
  void updateChannel(Channel* channel);
  //删除不需要监听的事件描述符
  void removeChannel(Channel* channel);
  void assertInLoopThread();
private:
  static const int kInitEventListSize = 16;
  void fillActiveChannels(int numEvents,ChannelList *activeChannels) const;
  void update(int operation,Channel *channel);

  EventLoop *loop_;
  int epollfd_;
  //当有事件发生时，进行填充
  EventList events_;
  //只是用作查找以及检测
  ChannelMap channels_;

};

#endif /* EPOLLER_H_ */
