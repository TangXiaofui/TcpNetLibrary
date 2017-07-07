/*
 * netAddress.h
 *
 *  Created on: Jul 7, 2017
 *      Author: txh
 */

#ifndef NETADDRESS_H_
#define NETADDRESS_H_

#include "utility.h"
#include <string>
#include <netinet/in.h>

class NetAddress:public copyable{
public:
  explicit NetAddress(uint16_t port);
  NetAddress(const std::string& ip,uint16_t port);
  NetAddress(const struct sockaddr_in &addr);

  //必须设置成const，否则当有const对象要使用时会出现错误
  std::string toHostPort() const;

  const struct sockaddr_in& getSockAddr() const;
  void setSockAddr(const struct sockaddr_in& addr);


private:
  struct sockaddr_in addr_;
};



#endif /* NETADDRESS_H_ */
