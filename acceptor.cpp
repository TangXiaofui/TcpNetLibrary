/*
 * acceptor.cpp
 *
 *  Created on: Jul 7, 2017
 *      Author: txh
 */


#include "acceptor.h"
#include "eventLoop.h"
#include "logging.h"
#include <sys/socket.h>
#include <unistd.h>

int createNoBlockSock()
{
  int sockfd = ::socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC,IPPROTO_TCP);
  if(sockfd < 0)
    {
      log_fatal("create nonblock socket failed");
    }
  return sockfd;
}

Acceptor::Acceptor(EventLoop *loop, const NetAddress &listenAddr)
:loop_(loop),
acceptSock_(createNoBlockSock()),
acceptChannel_(loop,acceptSock_.fd()),
listenning_(false)
{
  acceptSock_.setReuseAddr(true);
  acceptSock_.bindAddress(listenAddr);
  acceptChannel_.setReadCallBack(std::bind(&Acceptor::handleRead,this,std::placeholders::_1));
}

void Acceptor::setNewConnectCallBack(const newConnCallBack &cb)
{
  newConnCallBack_ = cb;
}

bool Acceptor::listenning()
{
  return listenning_;
}

void Acceptor::listen()
{
  loop_->assertInLoopThread();
  listenning_ = true;
  acceptSock_.listen();
  acceptChannel_.enableReading();
}


void Acceptor::handleRead(TimeStamp receiveTime)
{
  loop_->assertInLoopThread();
  NetAddress peerAddr(0);
  int connfd = acceptSock_.accept(&peerAddr);
  if(connfd >= 0)
    {
      if(newConnCallBack_)
	{
	  newConnCallBack_(connfd,peerAddr);
	}
    }
  else
    {
      ::close(connfd);
    }
}


