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
  using ReadEventCallback = std::function<void(TimeStamp)>;
  Channel(EventLoop* loop,int fd);
  ~Channel();

  void handleEvent(TimeStamp receiveTime);
  void setReadCallBack(const ReadEventCallback &cb );
  void setWriteCallBack(const EventCallback &cb);
  void setErrorCallBack(const EventCallback &cb);
  void setCloseCallBack(const EventCallback &cb);

  int events() const;
  void setEvent(int revent);
  void enableReading();

  void enableWriting();
  void disableWriting();
  bool isWriting();

  void disableAll();

  int fd();
  int index();
  void setIndex(int idx);
  bool isNoneEvent() const;
  EventLoop* ownerLoop();


private:
  //决定构造函数的初始化顺序
  void update();
  static const int kNoneEvent;
  static const int kReadEvent;
  static const int kWriteEvent;

  EventLoop *loop_;
  const int fd_;
  int event_;
  int revent_;
  int index_;
  bool eventHanding_;

  ReadEventCallback readCallBack_;
  EventCallback writeCallBack_;
  EventCallback errorCallBack_;
  EventCallback closeCallBack_;
};



#endif /* CHANNEL_H_ */
