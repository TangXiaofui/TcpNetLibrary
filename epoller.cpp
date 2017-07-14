/*
 * epoller.cpp
 *
 *  Created on: Jul 13, 2017
 *      Author: txh
 */

#include "epoller.h"
#include "eventLoop.h"
#include "channel.h"
#include "logging.h"
#include <sys/epoll.h>
#include <unistd.h>
#include <assert.h>
#include <strings.h>


namespace {
  const int kNew = -1;
  const int kAdded = 1;
  const int kDelete = -2;
}
const int Epoller::kInitEventListSize;

Epoller::Epoller(EventLoop *loop)
:loop_(loop),
 epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
 events_(kInitEventListSize)
{
  if(epollfd_ < 0)
    {
      log_fatal("Epoller create failed");
    }
}

Epoller::~Epoller()
{
  ::close(epollfd_);
}

TimeStamp Epoller::poll(int timeoutMs,ChannelList* activeChannels)
{
  int numEvent = ::epoll_wait(epollfd_,&*events_.begin(),static_cast<int>(events_.size()),timeoutMs);
  TimeStamp now (TimeStamp::now());
  if(numEvent > 0)
    {
      log_trace("%d events happened",numEvent);
      fillActiveChannels(numEvent,activeChannels);
      if(implicit_cast<size_t>(numEvent) == events_.size())
	{
	  events_.resize(events_.size() * 2);
	}
    }
  else if(numEvent == 0)
    {
      log_trace("nothing happened");
    }
  else
    {
      log_error("Epoller poll failed");
    }
  return now;
}

void Epoller::fillActiveChannels(int numEvents,ChannelList *activeChannels) const
{
  assert(implicit_cast<size_t>(numEvents) <= events_.size());
  for(int i = 0; i < numEvents ; i++)
    {
      Channel *channel = static_cast<Channel*>(events_[i].data.ptr);
      int fd = channel->fd();
      ChannelMap::const_iterator it = channels_.find(fd);
      assert(it != channels_.end());
      assert(it->second == channel);

      channel->setEvent(events_[i].events);
      activeChannels->push_back(channel);
    }
}

void Epoller::updateChannel(Channel* channel)
{
  assertInLoopThread();
  log_trace("fd %d events : %d",channel->fd(),channel->events());
  const int index = channel->index();
  if(index == kNew || index == kDelete)
    {
      int fd = channel->fd();
      if(index ==kNew)
	{
	  assert(channels_.find(fd) == channels_.end());
	  channels_[fd] = channel;
	}
      else
	{
	  assert(channels_.find(fd) != channels_.end());
	  assert(channels_[fd] == channel);
	}
      channel->setIndex(kAdded);
      update(EPOLL_CTL_ADD,channel);
    }
  else
    {
      int fd = channel->fd();
      assert(channels_.find(fd) != channels_.end());
      assert(channels_[fd] == channel);
      if(channel->isNoneEvent())
	{
	  update(EPOLL_CTL_DEL,channel);
	  channel->setIndex(kDelete);
	}
      else
	{
	  update(EPOLL_CTL_MOD,channel);
	}
    }
}

void Epoller::removeChannel(Channel* channel)
{
  assertInLoopThread();
  int fd = channel->fd();
  log_trace("remove channle fd %d",fd);
  assert(channels_.find(fd) != channels_.end());
  assert(channels_[fd] == channel);
  assert(channel->isNoneEvent());
  int index = channel->index();
  assert(index == kAdded || index == kDelete);
  size_t n = channels_.erase(fd);
  assert(n == 1);

  if(index == kAdded)
    {
      update(EPOLL_CTL_DEL,channel);
    }
  channel->setIndex(kNew);
}

void Epoller::assertInLoopThread()
{
  loop_->assertInLoopThread();
}



void Epoller::update(int operation,Channel *channel)
{
  struct epoll_event event;
  ::bzero(&event,sizeof event);
  event.events = channel->events();
  event.data.ptr = channel;
  int fd = channel->fd();
  if(::epoll_ctl(epollfd_, operation, fd, &event))
    {
      if(operation == EPOLL_CTL_DEL)
	log_error("epoll delete channel fd %d failed",fd);
      else
	{
	  log_fatal("epoll update failed. fd = %d operation = %d",fd,operation);
	}
    }

}


