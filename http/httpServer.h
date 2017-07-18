/*
 * httpServer.h
 *
 *  Created on: Jul 18, 2017
 *      Author: txh
 */

#ifndef HTTPSERVER_H_
#define HTTPSERVER_H_

#include "../tcpServer.h"
#include "../utility.h"
#include "httpContext.h"
#include "httpResponse.h"
#include <functional>


class HttpServer:public noncopyable
{
public:
  using HttpCallback = std::function<void (const HttpRequest&,HttpResponse *)>;

  HttpServer(EventLoop *loop,const NetAddress &listenAddr,const string &name);
  ~HttpServer();

  EventLoop *getLoop() const;

  void setHttpCallback(const HttpCallback & cb);

  void setThreadNum(int numThreads);

  void start();

private:
  void onConnection(const TcpConnectionPtr &conn);
  void onMessage(const TcpConnectionPtr &conn,Buffer *buf,TimeStamp receiveTime);
  void onRequest(const TcpConnectionPtr &conn, const HttpRequest& req);

  TcpServer server_;
  HttpCallback httpCallback_;
};



#endif /* HTTPSERVER_H_ */
