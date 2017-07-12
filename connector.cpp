/*
 * connector.cpp
 *
 *  Created on: Jul 11, 2017
 *      Author: txh
 */

#include "connector.h"
#include <assert.h>
#include <unistd.h>
#include "eventLoop.h"
#include "channel.h"
#include "logging.h"
#include "socket.h"


const int Connector::kMaxRetryDelay;

Connector::Connector(EventLoop *loop, const NetAddress &serverAddr)
:loop_(loop),
 serverAddr_(serverAddr),
 connect_(false),
 state_(kDisconnect),
 retryDelayMs_(kInitRetryDelay)
{
  log_info("Connector serverAddr: %s cnostruction",serverAddr.toHostPort().c_str());
}

Connector::~Connector()
{
  log_debug("Connector serverAddr:%s destruction",serverAddr_.toHostPort().c_str());
  loop_->cancel(timerId_);
}

void Connector::setNewConnectionCallBack(const newConnectionCallBack &cb)
{
  connectCallback_ = cb;
}


void Connector::restart()
{
  loop_->assertInLoopThread();
  setState(kDisconnect);
  retryDelayMs_ = kInitRetryDelay;
  connect_ = true;
  startInloop();
}

void Connector::stop()
{
  connect_ = false;
}

const NetAddress& Connector::getServerAddress()
{
  return serverAddr_;
}

void Connector::setState(Status e)
{
  state_ = e;
}

void Connector::start()
{
  connect_ = true;
  loop_->runInLoop(std::bind(&Connector::startInloop,this));
}

void Connector::startInloop()
{
  loop_->assertInLoopThread();
  assert(state_ == kDisconnect);
  if(connect_)
    {
      connect();
    }
  else{
      log_debug("connect state is disconnect");
  }
}

void Connector::connect()
{
  int fd = createNoBlockSock();
  const struct sockaddr_in &addr = serverAddr_.getSockAddr();
  int ret = ::connect(fd,sockaddr_cast(&addr),sizeof addr);
  int saveError = (ret == 0)? 0 :errno;
  switch(saveError){
	case 0:
	//The socket is nonblocking and the connection cannot be completed  immediately.
        case EINPROGRESS:
        // The system call was interrupted by a signal that was caught
        case EINTR:
        //The socket is already connected
        case EISCONN:
          connecting(fd);
          break;

         //No more free local ports or insufficient entries in the routing  cache
        case EAGAIN:
         // Local address is already in use
        case EADDRINUSE:

        case EADDRNOTAVAIL:
          //No-one listening on the remote address
        case ECONNREFUSED:
//          Network is unreachable
        case ENETUNREACH:
          retry(fd);
          break;

        case EACCES:
        case EPERM:
        case EAFNOSUPPORT:
        case EALREADY:
        case EBADF:
        case EFAULT:
        case ENOTSOCK:
              log_error("connect error in Connector::startInLoop %d", saveError);
              ::close(fd);
              break;

            default:
              log_error("Unexpected error in Connector::startInLoop %d",saveError);
             ::close(fd);
              break;

  }

}

void Connector::connecting(int sockfd)
{
  setState(kConnecting);
  assert(!channel_);
  channel_.reset(new Channel(loop_,sockfd));
  channel_->setWriteCallBack(std::bind(&Connector::handleWrite,this));
  channel_->setErrorCallBack(std::bind(&Connector::handleError,this));
  channel_->enableWriting();
}

void Connector::handleWrite()
{
  log_info("Connector handleWrite serverAddr: %s",serverAddr_.toHostPort().c_str());
  if(state_ == kConnecting)
    {
      int sockfd = removeAndResetChannel();
      int err = getSocketError(sockfd);
      if(err)
	{
	   log_warn("Connector handleWrite SO_ERROR , errno:%d, $s",err,strerror_tl(err));
	   retry(sockfd);
	}
      else if(isSelfConnect(sockfd))
	{
	  log_warn("Connector handleWrite self connection");
	  retry(sockfd);
	}
      else{
	  setState(kConnected);
	  if(connect_)
	    {
	      connectCallback_(sockfd);
	    }
	  else
	    {
	      close(sockfd);
	    }
      }
    }
}

void Connector::handleError()
{
  log_error("Connector handleError");

  int sockfd = removeAndResetChannel();
  int err = getSocketError(sockfd);
  log_error("SO_ERROR %d %s",err,strerror_tl(err));
  retry(sockfd);
}

void Connector::retry(int sockfd)
{
  ::close(sockfd);
  setState(kDisconnect);
  if(connect_)
    {
      log_info("Connector retry - Retry connect to %s in %d",serverAddr_.toHostPort().c_str(),retryDelayMs_);
      timerId_ = loop_->runAfter(retryDelayMs_/1000.0,std::bind(&Connector::startInloop,this));
      retryDelayMs_ = std::min(retryDelayMs_ * 2, kMaxRetryDelay);
    }
  else
    {
      log_debug("connect failed");
    }
}

int Connector::removeAndResetChannel()
{
  channel_->disableAll();
  int sockfd = channel_->fd();
  loop_->removeChannel(channel_.get());
  loop_->queueInloop(std::bind(&Connector::resetChannel,this));
  return sockfd;
}

void Connector::resetChannel()
{
  channel_.reset();
}




