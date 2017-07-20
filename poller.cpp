/*
 * poller.cpp
 *
 *  Created on: Jul 5, 2017
 *      Author: txh
 */

#include "poller.h"
#include "channel.h"
#include "logging.h"
#include "eventLoop.h"
#include <poll.h>
#include <assert.h>

Poller::Poller(EventLoop *loop)
:loop_(loop)
{

}

Poller::~Poller()
{
  //不清除fd，由生成fd的具体类自己管理fd
}

TimeStamp Poller::poll(int timeoutMs,ChannelList* activeChannels)
{
  int nums = ::poll(&*pollfds_.begin(),pollfds_.size(),timeoutMs);
  TimeStamp now = TimeStamp::now();
  if(nums > 0){
      log_trace("%d event happen",nums);
      fillActiveChannels(nums,activeChannels);
  }
  else if(nums == 0){
      log_trace("nothing happen");
  }
  else{
      log_error("Poller::poll error");
  }
  return now;
}

void Poller::updateChannel(Channel *channel)
{
  assertInLoopThread();
  log_trace("fd = %d,event = %d",channel->fd(),channel->events());
  if(channel->index() < 0)
    {
      assert(channels_.find(channel->fd()) == channels_.end());
      struct pollfd pfd;
      pfd.fd = channel->fd();
      pfd.events = static_cast<short>(channel->events());
      pfd.revents = 0;
      pollfds_.push_back(pfd);
      int idx = static_cast<int>(pollfds_.size())-1;
      channel->setIndex(idx);
      channels_[pfd.fd] = channel;
    }
  else
    {
      assert(channels_.find(channel->fd()) != channels_.end());
      assert(channels_[channel->fd()] == channel);
      int idx = channel->index();
      assert(idx >= 0 && idx < static_cast<int>(pollfds_.size()));
      struct pollfd& pfd = pollfds_[idx];
      assert(pfd.fd == channel->fd() || pfd.fd == -channel->fd()-1);
      pfd.events = static_cast<short>(channel->events());
      pfd.revents = 0;
      if(channel->isNoneEvent()){
	  pfd.fd = -channel->fd()-1;
      }
    }
}

void Poller::removeChannel(Channel *channel)
{
  assertInLoopThread();
  assert(channels_.find(channel->fd())!= channels_.end());
  assert(channels_[channel->fd()] == channel);
  assert(channel->isNoneEvent());

  int idx = channel->index();
  assert(0 <= idx && idx < static_cast<int>(pollfds_.size()));
  size_t n = channels_.erase(channel->fd());
  assert(n == 1);
  if(implicit_cast<size_t>(idx) == pollfds_.size()-1)
    {
      pollfds_.pop_back();
    }
  else
    {
      //不能找到位置直接erase，否则后面的索引全部失效
      //只能与最后交换，然后pop_back
      //pollfds_.erase(pollfds_.begin()+idx);
      int channelAtEnd = pollfds_.back().fd;
      if(channelAtEnd < 0)
	{
	  channelAtEnd = -channelAtEnd-1;
	}
      iter_swap(pollfds_.begin()+idx,pollfds_.end()-1);
      channels_[channelAtEnd]->setIndex(idx);
      pollfds_.pop_back();
    }
}

void Poller::assertInLoopThread()
{
  loop_->assertInLoopThread();
}


void Poller::fillActiveChannels(int nums,ChannelList* activeChannels) const
{
  for(PollerList::const_iterator pfd = pollfds_.begin(); pfd != pollfds_.end() && nums > 0; pfd++ ){
      if(pfd->revents > 0)
	{
	   nums--;
	   ChannelMap::const_iterator ch = channels_.find(pfd->fd);
	   assert(ch != channels_.end());
	   Channel *channel = ch->second;
	   channel->setEvent(pfd->revents);
	   activeChannels->push_back(channel);
	}
  }
}


