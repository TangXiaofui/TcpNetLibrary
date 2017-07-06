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

}

TimeStamp Poller::poll(int timeoutMs,ChannelList* activeChannels)
{
  int nums = ::poll(&*pollfds_.begin(),pollfds_.size(),timeoutMs);
  TimeStamp now = TimeStamp::now();
  if(nums > 0){
      trace("%d event happen",nums);
      fillActiveChannels(nums,activeChannels);
  }
  else if(nums == 0){
      trace("nothing happen");
  }
  else{
      error("Poller::poll error");
  }
  return now;
}

void Poller::updateChannel(Channel *channel)
{
  assertInLoopThread();
  trace("fd = %d,event = %d",channel->fd(),channel->events());
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
      assert(pfd.fd == channel->fd() || pfd.fd == -1);
      pfd.events = static_cast<short>(channel->events());
      pfd.revents = 0;
      if(channel->isNoneEvent()){
	  pfd.fd = -1;
      }
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


