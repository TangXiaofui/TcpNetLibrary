/*
 * poller.h
 *
 *  Created on: Jul 5, 2017
 *      Author: txh
 */

#ifndef POLLER_H_
#define POLLER_H_

#include "utility.h"
#include "timeStamp.h"
#include <vector>
#include <map>



class EventLoop;
struct pollfd;
class Channel;

class Poller:public noncopyable{
public:
  using ChannelList = std::vector<Channel*>;
  using ChannelMap = std::map<int,Channel*>;
  using PollerList = std::vector<struct pollfd>;
  Poller(EventLoop *loop);
  ~Poller();

  TimeStamp poll(int timeoutMs,ChannelList* activeChannels);
  void updateChannel(Channel *channel);
  void removeChannel(Channel *channel);
  void assertInLoopThread();

private:
  void fillActiveChannels(int nums,ChannelList* activeChannels) const;

  EventLoop *loop_;
  //需要传入poll
  PollerList pollfds_;
  //该map主要用于fillActiveChannels中快速查找
  ChannelMap channels_;

};


#endif /* POLLER_H_ */
