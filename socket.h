/*
 * socket.h
 *
 *  Created on: Jul 7, 2017
 *      Author: txh
 */

#ifndef SOCKET_H_
#define SOCKET_H_

#include "utility.h"

class NetAddress;


class Socket:public noncopyable{
public:
  explicit Socket(int fd);
  ~Socket();

  int fd() const;

  void bindAddress(const NetAddress& localAddr);
  void listen();
  int accept(NetAddress *peerAddr);
  void setTcpNoDelay(bool flag);

  void setReuseAddr(bool flag);

  void shutdownWrite();
private:
  const int fd_;
};

struct sockaddr_in getLocalAddr(int sockfd);

#endif /* SOCKET_H_ */
