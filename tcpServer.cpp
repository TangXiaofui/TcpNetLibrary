/*
 * tcpServer.cpp
 *
 *  Created on: Jul 7, 2017
 *      Author: txh
 */


#include "tcpServer.h"
#include "eventLoop.h"
#include "acceptor.h"
#include "logging.h"
#include "eventLoopThreadPool.h"
#include <assert.h>

TcpServer::TcpServer(EventLoop *loop, const NetAddress &addr)
:loop_(loop),
name_(addr.toHostPort()),
acceptor_(new Acceptor(loop,addr)),
threadpoll_(new EventLoopThreadPool(loop)),
started_(false),
nextConnId_(1)
{
  acceptor_->setNewConnectCallBack(
      std::bind(&TcpServer::newConnection,this,std::placeholders::_1,std::placeholders::_2));
}

TcpServer::~TcpServer()
{

}

void TcpServer::start()
{
  if(!started_)
    {
      started_ = true;
      threadpoll_->start();
    }
  if(!acceptor_->listenning())
    {
      loop_->runInLoop(std::bind(&Acceptor::listen,acceptor_.get()));
    }

}

void TcpServer::setConnectionCallBack(const ConnectionCallBack& cb)
{
  connectCallBack_ = cb;
}

void TcpServer::setMessageCallBack(const MessageCallBack& cb)
{
  messageCallBack_ = cb;
}

void TcpServer::newConnection(int sockfd, const NetAddress &addr)
{
  loop_->assertInLoopThread();
  char buf[32];
  snprintf(buf,sizeof buf,"#%d",nextConnId_);
  nextConnId_++;
  std::string connName = name_ + buf;
  log_info("TcpServer newConnect %s ,new Connection %s from peer %s",name_.c_str(),connName.c_str(),addr.toHostPort().c_str());

  NetAddress localAddr(getLocalAddr(sockfd));


  EventLoop* ioLoop = threadpoll_->getNextLoop();

  TcpConnectionPtr conn = std::make_shared<TcpConnection>(ioLoop,connName,sockfd,localAddr,addr);
  connects_[connName] = conn;
  conn->setConnectedCallBack(connectCallBack_);
  conn->setMessageCallBack(messageCallBack_);
  conn->setCloseCallBack(std::bind(&TcpServer::removeConnection,this,std::placeholders::_1));

  ioLoop->runInLoop(std::bind(&TcpConnection::connectEestablished,conn));
}


void TcpServer::removeConnection(const TcpConnectionPtr& conn)
{
  loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop,this,conn));
}


void TcpServer::setThreadNum(int numThreads)
{
  assert(0 <= numThreads);
  threadpoll_->setThreadNum(numThreads);
}

void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn)
{
  loop_->assertInLoopThread();
  size_t n = connects_.erase(conn->getName());
  assert(n == 1);
  EventLoop* ioLoop = conn->getEventLoop();
  ioLoop->queueInloop(std::bind(&TcpConnection::connectDestory,conn));
}
