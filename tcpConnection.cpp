/*
 * tcpConnection.cpp
 *
 *  Created on: Jul 7, 2017
 *      Author: txh
 */


#include "tcpConnection.h"
#include "eventLoop.h"
#include "logging.h"
#include <assert.h>
#include <unistd.h>

TcpConnection::TcpConnection(EventLoop *loop, const std::string& name, int sockfd,\
		    const NetAddress &localaddr,const NetAddress &peerAddr)
:loop_(loop),
name_(name),
state_(kConnecting),
socket_(new Socket(sockfd)),
channel_(new Channel(loop,sockfd)),
localAddr_(localaddr),
peerAddr_(peerAddr)
{
  log_debug("TcpConnetion %s create at fd = %d",name.c_str(),sockfd);
  channel_->setReadCallBack(std::bind(&TcpConnection::handleRead,this));
}

TcpConnection::~TcpConnection()
{
  log_debug("TcpConnection %s close at fd = %d",name_.c_str(),channel_->fd());
}

EventLoop* TcpConnection::getEventLoop() const
{
  return loop_;
}

const std::string& TcpConnection::getName() const
{
  return name_;
}

const NetAddress& TcpConnection::getLocalAddr() const
{
  return localAddr_;
}

const NetAddress& TcpConnection::getPeerAddr() const
{
  return peerAddr_;
}

bool TcpConnection::isConneted()
{
  return state_ == kConnected;
}

void TcpConnection::setConnectedCallBack(const ConnectionCallBack& cb)
{
  connectionCallBack_ = cb;
}
void TcpConnection::setMessageCallBack(const MessageCallBack& cb)
{
  messageCallBack_ = cb;
}

void TcpConnection::connectEestablished()
{
  loop_->assertInLoopThread();
  assert(state_ == kConnecting);
  setState(kConnected);
  channel_->enableReading();

  if(connectionCallBack_)
    connectionCallBack_(shared_from_this());
}

void TcpConnection::setState(StateE state)
{
  state_ = state;
}

void TcpConnection::handleRead()
{
  char buf[65536];
  ssize_t ret = ::read(channel_->fd(),buf,sizeof buf);
  messageCallBack_(shared_from_this(),buf,ret);
}

