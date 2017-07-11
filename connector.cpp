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





Connector::Connector(EventLoop *loop, const NetAddress &serverAddr)
:loop_(loop),
 serverAddr_(serverAddr),
 connect_(false),
 state_(kDisconnect),
 retryDelayMs_(kInitRetryDelay)
{

}

Connector::~Connector()
{

}

void Connector::setNewConnectionCallBack(const newConnectionCallBack &cb)
{
  connectCallback_ = cb;
}

void Connector::start()
{
  connect_ = true;
  loop_->runInLoop(std::bind(&Connector::startInloop,this));
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
void Connector::startInloop()
{
  loop_->assertInLoopThread();
  assert(state_ == kDisconnect);
  if(connect_)
    {
      connect();
    }
  else{
      log_debug("not connect");
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

}

void Connector::handleWrite()
{

}

void Connector::handleError()
{

}

void Connector::retry(int sockfd)
{

}

void Connector::removeAndResetChannel()
{

}

void Connector::resetChannel()
{

}




