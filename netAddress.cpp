/*
 * netAddress.cpp
 *
 *  Created on: Jul 7, 2017
 *      Author: txh
 */


#include "netAddress.h"
#include <string.h>
#include <arpa/inet.h>
#include "logging.h"

NetAddress::NetAddress(uint16_t port)
{
  ::bzero(&addr_,sizeof addr_);
  addr_.sin_family = AF_INET;
  addr_.sin_addr.s_addr = htonl(INADDR_ANY);
  addr_.sin_port = htons(port);
}

NetAddress::NetAddress(const std::string& ip,uint16_t port)
{
  ::bzero(&addr_,sizeof addr_);
  addr_.sin_family = AF_INET;
  if(::inet_pton(AF_INET,ip.c_str(),&addr_.sin_addr) <= 0)
    {
      log_error("inet_pton failed");
    }
  addr_.sin_port = htons(port);
}

NetAddress::NetAddress(const struct sockaddr_in &addr)
:addr_(addr)
{

}

std::string NetAddress::toHostPort() const
{
  char buf[32];
  char host[INET_ADDRSTRLEN] = "INVALID";
  ::inet_ntop(AF_INET,&addr_.sin_addr,host,sizeof host);
  short port = ntohs(addr_.sin_port);
  snprintf(buf,sizeof host,"%s:%u",host,port);
  return buf;
}

const struct sockaddr_in& NetAddress::getSockAddr() const
{
  return addr_;
}
void NetAddress::setSockAddr(const struct sockaddr_in& addr)
{
  addr_ = addr;
}


