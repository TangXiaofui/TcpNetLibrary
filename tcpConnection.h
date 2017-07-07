/*
 * tcpConnection.h
 *
 *  Created on: Jul 7, 2017
 *      Author: txh
 */

#ifndef TCPCONNECTION_H_
#define TCPCONNECTION_H_

#include "utility.h"
#include "channel.h"
#include "socket.h"
#include "netAddress.h"
#include <memory>
#include <string>
#include <functional>
class EventLoop;

class TcpConnection:public noncopyable,public std::enable_shared_from_this<TcpConnection>{
public:


    TcpConnection(EventLoop *loop, const std::string& name, int sockfd, \
			const NetAddress &localaddr,const NetAddress &peerAddr);
    ~TcpConnection();

    EventLoop* getEventLoop() const;
    const std::string& getName() const;
    const NetAddress& getLocalAddr() const;
    const NetAddress& getPeerAddr() const;
    bool isConneted();

    void setConnectedCallBack(const ConnectionCallBack& cb);
    void setMessageCallBack(const MessageCallBack& cb);

    void connectEestablished();
private:
    enum StateE{ kConnecting,kConnected};
    void setState(StateE state);
    void handleRead();

    EventLoop *loop_;
    std::string name_;
    StateE state_;

    std::shared_ptr<Socket> socket_;
    std::shared_ptr<Channel> channel_;
    NetAddress localAddr_;
    NetAddress peerAddr_;

    ConnectionCallBack connectionCallBack_;
    MessageCallBack messageCallBack_;
};


#endif /* TCPCONNECTION_H_ */