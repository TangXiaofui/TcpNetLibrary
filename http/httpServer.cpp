/*
 * httpServer.cpp
 *
 *  Created on: Jul 18, 2017
 *      Author: txh
 */


#include "httpServer.h"
#include "../logging.h"


void defaultHttpCallback(const HttpRequest& , HttpResponse *resp)
{
  resp->setStatusCode(HttpResponse::k404NotFound);
  resp->setStatusMessage("Not Found");
  resp->setCloseConnection(true);
}


HttpServer::HttpServer(EventLoop *loop,const NetAddress &listenAddr,const string &name)
:server_(loop,listenAddr),
 httpCallback_(defaultHttpCallback)
{
  //设置连接时回调
  server_.setConnectionCallBack(std::bind(&HttpServer::onConnection,this,std::placeholders::_1));
  //设置收到数据的回调函数
  server_.setMessageCallBack(std::bind(&HttpServer::onMessage,this,std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));

}
HttpServer::~HttpServer()
{

}

EventLoop* HttpServer::getLoop() const
{
  return server_.getLoop();
}

void HttpServer::setHttpCallback(const HttpCallback & cb)
{
  httpCallback_ = cb;
}

void HttpServer::setThreadNum(int numThreads)
{
  server_.setThreadNum(numThreads);
}

void HttpServer::start()
{
  log_info("httpserver start");
  server_.start();
}


void HttpServer::onConnection(const TcpConnectionPtr &conn)
{

}

void HttpServer::onMessage(const TcpConnectionPtr &conn,Buffer *buf,TimeStamp receiveTime)
{
  //收到信息时，进行解析，成功则回调用户回调函数，否则返回错误，并关闭链接
  //注意下面的可变对象
  HttpContext &context = conn->getContext<HttpContext>();
  if(!context.parseRequest(buf,receiveTime))
    {
      conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
      conn->shutdown();
    }
  if(context.gotAll())
    {
      onRequest(conn,context.request());
      context.reset();
    }
}

void HttpServer::onRequest(const TcpConnectionPtr &conn, const HttpRequest& req)
{
  const string& connection = req.getHeader("Connection");
  //http1.0不支持长连接，http1.1支持,若收到用户请求信息内请求关闭链接，则发完信息后马上进行关闭链接操作
  bool close = (connection == "close") || (req.getVersion() == HttpRequest::kHttp10 && connection != "Keep-Alive");
  HttpResponse response(close);
  httpCallback_(req,&response);
  Buffer buf;
  response.appendTobuffer(&buf);
  conn->send(&buf);
  if(response.closeConnection())
    {
      conn->shutdown();
    }

}



