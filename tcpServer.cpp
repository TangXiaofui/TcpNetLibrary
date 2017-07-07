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


TcpServer::TcpServer(EventLoop *loop, const NetAddress &addr)
:loop_(loop),
name_(addr.toHostPort()),
acceptor_(new Acceptor(loop,addr)),
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
    started_ = true;

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
  TcpConnectionPtr conn = std::make_shared<TcpConnection>(loop_,connName,sockfd,localAddr,addr);
  connects_[connName] = conn;
  conn->setConnectedCallBack(connectCallBack_);
  conn->setMessageCallBack(messageCallBack_);
  conn->connectEestablished();
}


