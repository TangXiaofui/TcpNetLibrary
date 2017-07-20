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
  channel_->setReadCallBack(std::bind(&TcpConnection::handleRead,this,std::placeholders::_1));
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
  assert(state_ == kConnected || state_ == kDisconnecting);
  setState(kDisconnected);
  channel_->disableAll();

  if(connectionCallBack_)
    connectionCallBack_(shared_from_this());

  loop_->removeChannel(channel_.get());

}

void TcpConnection::handleRead(TimeStamp receiveTime)
{
  int saveErrno = 0;
  ssize_t ret = inputBuffer_.readFd(channel_->fd(),&saveErrno);
  if(ret > 0)
    messageCallBack_(shared_from_this(),&inputBuffer_,receiveTime);
  else if(ret == 0){
      handleClose();
  }
  else{
      handleError();
  }
}


void TcpConnection::handleWrite()
{
  loop_->assertInLoopThread();
  if(channel_->isWriting()){
      ssize_t n = ::write(channel_->fd(),outputBuffer_.peek(),outputBuffer_.readableBytes());
      if(n > 0){
	  outputBuffer_.retrieve(n);
	  //若buffer写完，则马上取消写事件，防止不断的出发写事件，造成busyloop
	  if(outputBuffer_.readableBytes() == 0)
	    {
	      channel_->disableWriting();
	      if(state_ == kDisconnecting){
		  shutdownInloop();
	      }
	    }
	  else{
	      log_trace("I am going write more data");
	  }
      }
      else{
	  log_error("TcpConnect handleWrite");
      }
  }
  else{
      log_trace("connection is down,no more writing");
  }
}

void TcpConnection::handleClose()
{
   loop_->assertInLoopThread();
   log_info("tcpConnection handleClose");
   assert(state_ == kConnected || state_ == kDisconnecting);
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


void TcpConnection::send(const std::string &message)
{
  if(state_ == kConnected)
    {
      if(loop_->isInLoopThread())
	{
	  sendInloop(message);
	}
      else{
	  loop_->runInLoop(std::bind(&TcpConnection::sendInloop,this,message));
      }
    }
}

void TcpConnection::send(Buffer* message)
{
  string msg(message->peek(), message->readableBytes());
  message->retrieveAll();
  send(msg);
}

void TcpConnection::shutdown()
{
  if(state_ == kConnected)
    {
      setState(kDisconnecting);
      loop_->runInLoop(std::bind(&TcpConnection::shutdownInloop,this));
    }
}

void TcpConnection::sendInloop(const std::string &message)
{
  //当有数据需要发送时，先直接发送，若有还没发送则放入缓存区，并注册写事件。这样会不断出发写事件，然后调用handleWrite
  loop_->assertInLoopThread();
  ssize_t nWrote = 0;
  if(!channel_->isWriting() && outputBuffer_.readableBytes() == 0)
    {
      nWrote = ::write(channel_->fd(),message.c_str(),message.length());
      if(nWrote >= 0)
	{
	  if(implicit_cast<size_t>(nWrote) < message.length())
	    {
	      log_error("I am going to write more date");
	    }
	}
      else{
	  nWrote = 0;
	  if(errno != EWOULDBLOCK)
	    {
	      log_error("TcpConnect sendInloop");
	    }
      }
    }
  assert(nWrote >= 0);
  if(implicit_cast<size_t>(nWrote) < message.size())
    {
      outputBuffer_.append(message.data()+nWrote,message.length()-nWrote);
      if(!channel_->isWriting())
	{
	  channel_->enableWriting();
	}
    }
}

void TcpConnection::shutdownInloop()
{
  loop_->assertInLoopThread();
  if(!channel_->isWriting())
    {
      socket_->shutdownWrite();
    }
}

void TcpConnection::setTcpNoDelay(bool flag)
{
  socket_->setTcpNoDelay(flag);
}

