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
#include <sys/socket.h>

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
  channel_->setWriteCallBack(std::bind(&TcpConnection::handleWrite, this));
  channel_->setCloseCallBack(std::bind(&TcpConnection::handleClose, this));
  channel_->setErrorCallBack(std::bind(&TcpConnection::handleError,this));
}

TcpConnection::~TcpConnection()
{
  //dopendingFunctor中析构
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

void TcpConnection::setCloseCallBack(const CloseCallBack &cb)
{
  closeCallBack_ = cb;
}


void TcpConnection::setState(StateE state)
{
  state_ = state;
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

void TcpConnection::connectDestory()
{
  loop_->assertInLoopThread();
  assert(state_ == kConnected);
  setState(kDisconnected);
  channel_->disableAll();

  if(connectionCallBack_)
    connectionCallBack_(shared_from_this());

  loop_->removeChannel(channel_.get());

}

void TcpConnection::handleRead()
{
  char buf[65536];
  ssize_t ret = ::read(channel_->fd(),buf,sizeof buf);
  if(ret > 0)
    messageCallBack_(shared_from_this(),buf,ret);
  else if(ret == 0){
      handleClose();
  }
  else{
      handleError();
  }
}


void TcpConnection::handleWrite()
{

}

void TcpConnection::handleClose()
{
   loop_->assertInLoopThread();
   log_info("tcpConnection handleClose");
   assert(state_ == kConnected);
   channel_->disableAll();
   closeCallBack_(shared_from_this());
}

void TcpConnection::handleError()
{
  int optval;
  socklen_t optlen = sizeof optlen;
  int ret = ::getsockopt(channel_->fd(),SOL_SOCKET, SO_ERROR, &optval, &optlen);
  log_error("TcpConnect handleError %s ,socket error: %s",name_.c_str(),strerror_tl(ret < 0 ? errno : optval));
}

