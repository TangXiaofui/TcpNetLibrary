/*
 * channel.cpp
 *
 *  Created on: Jul 5, 2017
 *      Author: txh
 */



#include "channel.h"
#include "logging.h"
#include "eventLoop.h"
#include <poll.h>
#include <assert.h>

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;


Channel::Channel(EventLoop* loop,int fd)
:loop_(loop),
 fd_(fd),
 event_(0),
 revent_(0),
 index_(-1),
 eventHanding_(false)
{

}

Channel::~Channel()
{
  //出现错误能及时打印出错信息
  assert(!eventHanding_);
}


/*
 * POLLHUP, on the other hand, indicates that your file descriptor is valid, but that it's in a state where:
   A device has been disconnected, or a pipe or FIFO has been closed by the last process that had it open for
 writing.
   Once set, the hangup state of a FIFO shall persist until some process opens the FIFO for writing or until all
 read-only file descriptors for the FIFO are closed. This event and POLLOUT are mutually-exclusive; a stream
 can never be writable if a hangup has occurred. However, this event and POLLIN, POLLRDNORM, POLLRDBAND, or
 POLLPRI are not mutually-exclusive. This flag is only valid in the revents bitmask; it shall be ignored
 in the events member.
Source: http://pubs.opengroup.org/onlinepubs/9699919799/functions/poll.html
If you want to see POLLHUP, simply open a pipe, close the reading end, and query the writing end with poll.
*/
void Channel::handleEvent(TimeStamp receiveTime)
{
  eventHanding_ = true;
  //这是一个未打开的文件描述符
  if(revent_ & POLLNVAL)
    {
      log_warn("Chanel handleEvent pollnval");
    }
  if((revent_ & POLLHUP) && (revent_ & POLLIN))
    {
      log_warn("channel handle event pollup");
      if(closeCallBack_)
	closeCallBack_();
    }

  if(revent_ & (POLLNVAL | POLLERR))
    {
      if(errorCallBack_)
	errorCallBack_();
    }
  if(revent_ & (POLLIN | POLLPRI | POLLRDHUP))
    {
      if(readCallBack_)
	readCallBack_(receiveTime);
    }
  if(revent_ & (POLLOUT))
    {
      if(writeCallBack_)
	writeCallBack_();
    }
  eventHanding_ = false;
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
void Channel::disableAll()
{
  event_ = kNoneEvent;
  update();
}

void Channel::enableWriting()
{
  event_ |= kWriteEvent;
  update();
}
void Channel::disableWriting()
{
  event_ &= ~kWriteEvent;
  update();
}
bool Channel::isWriting()
{
  return event_ & kWriteEvent;
}


void Channel::setReadCallBack(const ReadEventCallback &cb )
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

void Channel::setCloseCallBack(const EventCallback &cb)
{
  closeCallBack_ = cb;
}

EventLoop* Channel::ownerLoop()
{
  return loop_;
}


void Channel::update()
{
  loop_->updateChannel(this);
}
