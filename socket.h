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

  void setReuseAddr(bool flag);

private:
  const int fd_;
};



#endif /* SOCKET_H_ */
