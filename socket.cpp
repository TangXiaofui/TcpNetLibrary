/*
 * socket.cpp
 *
 *  Created on: Jul 7, 2017
 *      Author: txh
 */

#include "socket.h"
#include "netAddress.h"
#include "logging.h"
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <sys/socket.h>

Socket::Socket(int fd)
:fd_(fd)
{

}

Socket::~Socket()
{
  if(::close(fd_) < 0)
    {
      log_error("close socket fd failed");
    }
}

int Socket::fd() const
{
  return fd_;
}

void Socket::bindAddress(const NetAddress& localAddr)
{
  const struct sockaddr_in &addr = localAddr.getSockAddr();
  if(::bind(fd_,sockaddr_cast(&addr),sizeof addr) < 0)
    {
      log_fatal("socket bind address fail");
    }
}

void Socket::listen()
{
  if(::listen(fd_,SOMAXCONN) < 0)
    {
      log_fatal("socket listen address fail");
    }
}

int Socket::accept(NetAddress *peerAddr)
{
   struct sockaddr_in addr;
   socklen_t len = sizeof addr;
   ::bzero(&addr,len);
   int connfd = accept4(fd_,sockaddr_cast(&addr),&len,SOCK_NONBLOCK|SOCK_CLOEXEC);
   if(connfd < 0){
	 int saveErrno = errno;
	 log_error("Socket accept failed");
	 switch(saveErrno){
	   case EAGAIN:
	   case EINTR:
	   case EPROTO:
	   case EPERM:
	   case EMFILE:
	     errno = saveErrno;
	     break;
	   case EBADF:
	   case EFAULT:
	   case EINVAL:
	   case ENFILE:
	   case ENOBUFS:
	   case ENOMEM:
	   case ENOTSOCK:
	   case EOPNOTSUPP:
	     log_fatal("unexpect error of accept");
	     break;
	   default:
	     log_fatal("unknow error of accept");
	     break;
	   }
     }
   else{
       peerAddr->setSockAddr(addr);
   }
   return connfd;
}


void Socket::setReuseAddr(bool flag)
{
  int optval = flag ? 1 : 0;
  ::setsockopt(fd_,SOL_SOCKET,SO_REUSEADDR,&optval,sizeof optval);
}

void Socket::shutdownWrite()
{
  if(::shutdown(fd_,SHUT_WR) < 0)
    {
      log_error("socket shutdownWrite");
    }
}
void Socket::setTcpNoDelay(bool flag)
{
  int optval = flag ? 1: 0;
  ::setsockopt(fd_,IPPROTO_TCP,TCP_NODELAY,&optval,sizeof optval);
}


struct sockaddr_in getLocalAddr(int sockfd)
{
  struct sockaddr_in addr;
  socklen_t len = sizeof addr;
  ::bzero(&addr,len);
  if(::getsockname(sockfd,sockaddr_cast(&addr),&len) < 0)
    {
      log_error("getsockname fail");
    }
  return addr;
}

struct sockaddr_in getPeerAddr(int sockfd)
{
  struct sockaddr_in peeraddr;
  bzero(&peeraddr,sizeof peeraddr);
  socklen_t len = sizeof peeraddr;
  if(::getpeername(sockfd,sockaddr_cast(&peeraddr),&len) < 0)
    {
      log_error("getpeername fail");
    }
  return peeraddr;
}

int createNoBlockSock()
{
  int sockfd = ::socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC,IPPROTO_TCP);
  if(sockfd < 0)
    {
      log_fatal("create nonblock socket failed");
    }
  return sockfd;
}

using SA = struct sockaddr;

const SA* sockaddr_cast(const struct sockaddr_in *addr)
{
  return static_cast<const SA*>(implicit_cast<const void*>(addr));
}

SA* sockaddr_cast(struct sockaddr_in *addr)
{
  return static_cast<SA*>(implicit_cast<void*>(addr));
}

int getSocketError(int sockfd)
{
  int optVal;
  socklen_t optlen = sizeof optlen;
  if(::getsockopt(sockfd,SOL_SOCKET,SO_ERROR, &optVal,&optlen))
    {
      return errno;
    }
  else
    {
      return optVal;
    }
}

bool isSelfConnect(int sockfd)
{
  struct sockaddr_in localaddr = getLocalAddr(sockfd);
  struct sockaddr_in peeraddr = getPeerAddr(sockfd);
  return localaddr.sin_addr.s_addr == peeraddr.sin_addr.s_addr
	&& localaddr.sin_port == peeraddr.sin_port;
}



