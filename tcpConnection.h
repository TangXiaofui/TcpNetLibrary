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
#include "buffer.h"
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
    void setCloseCallBack(const CloseCallBack &cb);

    void connectEestablished();
    void connectDestory();

    void send(const std::string &message);
    void send(Buffer* message);
    void shutdown();

    template<class T> T& getContext() { return context_.context<T>(); }

private:
    enum StateE{ kConnecting, kConnected, kDisconnecting ,kDisconnected};
    void setState(StateE state);
    void handleRead(TimeStamp receiveTime);
    void handleWrite();
    void handleClose();
    void handleError();
    void sendInloop(const std::string &message);
    void shutdownInloop();
    void setTcpNoDelay(bool flag);

    EventLoop *loop_;
    std::string name_;
    StateE state_;

    //用于管理sockfd生命周期
    std::shared_ptr<Socket> socket_;
    std::shared_ptr<Channel> channel_;
    NetAddress localAddr_;
    NetAddress peerAddr_;
    Buffer inputBuffer_;
    Buffer outputBuffer_;
    ConnectionCallBack connectionCallBack_;
    MessageCallBack messageCallBack_;
    CloseCallBack closeCallBack_;
    AutoContext context_;
};


#endif /* TCPCONNECTION_H_ */
