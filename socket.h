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

//对socket进行封装，管理sockfd生存周期
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
struct sockaddr_in getPeerAddr(int sockfd);

int createNoBlockSock();
using SA = struct sockaddr;
const SA* sockaddr_cast(const struct sockaddr_in *addr);
SA* sockaddr_cast(struct sockaddr_in *addr);
int getSocketError(int sockfd);
bool isSelfConnect(int sockfd);


#endif /* SOCKET_H_ */
