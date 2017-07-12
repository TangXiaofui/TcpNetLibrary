/*
 * connector.h
 *
 *  Created on: Jul 11, 2017
 *      Author: txh
 */

#ifndef CONNECTOR_H_
#define CONNECTOR_H_

#include <functional>
#include <memory>
#include "netAddress.h"
#include "utility.h"
#include "timerId.h"

class EventLoop;
class Channel;

class Connector : public noncopyable{
public:
  using newConnectionCallBack = std::function<void (int socked)>;
  Connector(EventLoop *loop, const NetAddress &serverAddr);
  ~Connector();

  void setNewConnectionCallBack(const newConnectionCallBack &cb);
  void start();
  void restart();
  void stop();

  const NetAddress& getServerAddress();

private:
  enum Status{kDisconnect, kConnecting, kConnected};
  static const int kMaxRetryDelay = 30 * 1000;
  static const int kInitRetryDelay = 500;

  void setState(Status e);
  void startInloop();
  void connect();
  void connecting(int sockfd);
  void handleWrite();
  void handleError();
  void retry(int sockfd);
  int removeAndResetChannel();
  void resetChannel();

  EventLoop *loop_;
  NetAddress serverAddr_;
  bool connect_;
  Status state_;
  std::shared_ptr<Channel> channel_;
  newConnectionCallBack connectCallback_;
  int retryDelayMs_;
  TimerId timerId_;

};


#endif /* CONNECTOR_H_ */
