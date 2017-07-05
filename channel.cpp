/*
 * channel.cpp
 *
 *  Created on: Jul 5, 2017
 *      Author: txh
 */



#include "channel.h"
#include <poll.h>
#include "logging.h"
#include "eventLoop.h"

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;


Channel::Channel(EventLoop* loop,int fd)
:loop_(loop),
 fd_(fd),
 event_(0),
 revent_(0),
 index_(-1)
{

}

Channel::~Channel()
{

}

void Channel::handleEvent()
{
  if(revent_ & POLLNVAL)
    {
      warn("Chanel handleEvent pollnval");
    }
  if(revent_ & (POLLNVAL | POLLERR))
    {
      if(errorCallBack_)
	errorCallBack_();
    }
  if(revent_ & (POLLIN | POLLPRI | POLLRDHUP))
    {
      if(readCallBack_)
	readCallBack_();
    }
  if(revent_ & (POLLOUT))
    {
      if(writeCallBack_)
	writeCallBack_();
    }

}

int Channel::events() const
{
  return event_;
}

void Channel::setEvent(int revent)
{
  revent_ = revent;
}

void Channel::setIndex(int idx)
{
  index_ = idx;
}

int Channel::index()
{
  return index_;
}
int Channel::fd()
{
  return fd_;
}


bool Channel::isNoneEvent() const
{
  return event_ == kNoneEvent;
}
void Channel::enableReading()
{
  event_ |= kReadEvent;
  update();
}

void Channel::setReadCallBack(const EventCallback &cb )
{
  readCallBack_ = cb;
}

void Channel::setWriteCallBack(const EventCallback &cb)
{
  writeCallBack_ = cb;
}
void Channel::setErrorCallBack(const EventCallback &cb)
{
  errorCallBack_ = cb;
}

EventLoop* Channel::ownerLoop()
{
  return loop_;
}


void Channel::update()
{
  loop_->updateChannel(this);
}
