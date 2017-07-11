/*
 * utility.cpp
 *
 *  Created on: Jul 5, 2017
 *      Author: txh
 */


#include "utility.h"
#include "logging.h"

#include <sys/syscall.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netinet/tcp.h>


long CurrentThread::tid()
{
  return syscall(SYS_gettid);
}

__thread char t_errnobuf[512];
const char* strerror_tl(int savedErrno)
{
  return strerror_r(savedErrno, t_errnobuf, sizeof t_errnobuf);
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

