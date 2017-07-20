/*
 * acceptor.h
 *
 *  Created on: Jul 7, 2017
 *      Author: txh
 */

#ifndef ACCEPTOR_H_
#define ACCEPTOR_H_

#include "utility.h"
#include "socket.h"
#include "netAddress.h"
#include "channel.h"

#include <functional>

class EventLoop;


//对socket类进行封装，用于服务端调用
class Acceptor:public noncopyable{
public:
  using newConnCallBack = std::function<void(int sockfd,const NetAddress&)>;

  Acceptor(EventLoop *loop, const NetAddress &listenAddr);

  void setNewConnectCallBack(const newConnCallBack &cb);

  bool listenning();
  void listen();

private:

  void handleRead(TimeStamp receiveTime);
  EventLoop *loop_;
  Socket acceptSock_;
  Channel acceptChannel_;
  newConnCallBack newConnCallBack_;
  bool listenning_;
};



#endif /* ACCEPTOR_H_ */
