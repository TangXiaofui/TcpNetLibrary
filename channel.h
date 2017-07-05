/*
 * channel.h
 *
 *  Created on: Jul 5, 2017
 *      Author: txh
 */

#ifndef CHANNEL_H_
#define CHANNEL_H_

#include "utility.h"
#include <functional>

class EventLoop;

class Channel:public noncopyable{
public:
  using EventCallback = std::function<void()>;

  Channel(EventLoop* loop,int fd);
  ~Channel();

  void handleEvent();
  void setReadCallBack(const EventCallback &cb );
  void setWriteCallBack(const EventCallback &cb);
  void setErrorCallBack(const EventCallback &cb);

  int events() const;
  void setEvent(int revent);
  void setIndex(int idx);
  int index();
  int fd();
  bool isNoneEvent() const;
  void enableReading();


  EventLoop* ownerLoop();


private:
  void update();
  static const int kNoneEvent;
  static const int kReadEvent;
  static const int kWriteEvent;

  EventLoop *loop_;
  const int fd_;
  int event_;
  int revent_;
  int index_;

  EventCallback readCallBack_;
  EventCallback writeCallBack_;
  EventCallback errorCallBack_;

};



#endif /* CHANNEL_H_ */
