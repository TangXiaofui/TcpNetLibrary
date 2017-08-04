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



void Channel::handleEvent(TimeStamp receiveTime)
{
  eventHanding_ = true;
  //这是一个未打开的文件描述符
  if(revent_ & POLLNVAL)
    {
      log_warn("Chanel handleEvent pollnval");
    }
  /*
   A POLLHUP means the socket is no longer connected. In TCP, this means FIN has been received and sent.
   https://stackoverflow.com/questions/24791625/how-to-handle-the-linux-socket-revents-pollerr-pollhup-and-pollnval
  */
  if((revent_ & POLLHUP) && (revent_ & POLLIN))
    {
      log_warn("channel handle event pollup");
      if(closeCallBack_)
	closeCallBack_();
    }
  /*
   * A POLLERR means the socket got an asynchronous error. In TCP, this typically means a RST has been received
   * or sent. If the file descriptor is not a socket, POLLERR might mean the device does not support polling.
  */
  if(revent_ & (POLLNVAL | POLLERR))
    {
      if(errorCallBack_)
	errorCallBack_();
    }

  /*
   *  POLLPRI
                     There is urgent data to read  (e.g.,  out-of-band  data  on  TCP
                     socket;  pseudoterminal  master  in  packet  mode has seen state
                     change in slave).

      POLLRDHUP (since Linux 2.6.17)
                     Stream socket peer closed connection, or shut down writing  half
                     of  connection.   The  _GNU_SOURCE  feature  test  macro must be
                     defined (before including any header files) in order  to  obtain
                     this definition.
   * */
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
