//============================================================================
// Name        : MultiThreadSafe.cpp
// Author      : txh
// Version     :
// Copyright   : 
// Description : Hello World in C, Ansi-style
//============================================================================

#include <sys/timerfd.h>
#include <string.h>
#include <memory>
#include <set>
#include <sys/syscall.h>
#include <typeinfo>	//typeid
#include <cassert>
#include <unistd.h>

//#include "UniqueLock.h"
//#include "Observable.h"
//#include "StockFactory.h"
//#include "singleton.h"
//#include "pthreadKey.h"
#include "utility.h"
#include "logging.h"
#include "eventLoop.h"
#include "channel.h"
#include "unitTest.h"
#include "eventLoopThread.h"
#include "netAddress.h"
#include "acceptor.h"
#include "tcpServer.h"

/*netlibrary，主要是框架设计（Reactor+io复用+线程池），优化策略（map，减小临界区,智能指针，RAII,线程间的通讯选择），可维护性，
*		可扩展性（模块化设计，降低耦合），可靠性（容错，日志系统，代码覆盖率），代码逻辑清晰（注释)
*
*/
EventLoop *globelLoop;


int main(void) {


   RunAllTests("testTcpServer");

   return EXIT_SUCCESS;
}

void onConnection(const TcpConnectionPtr& conn)
{
  if(conn->isConneted())
    log_info("new Connection %s from %s",conn->getName().c_str(),conn->getPeerAddr().toHostPort().c_str());
  else
    {
      log_info("connection %s is down",conn->getName().c_str());
    }

}
void onMessage(const TcpConnectionPtr& conn, const char* buf, ssize_t len)
{
  log_info("onMessage : receive msg(%ld) : %s from peer %s",len,buf,conn->getPeerAddr().toHostPort().c_str());
}

TEST(testTcpServer)
{
  EventLoop loop;
  TcpServer server(&loop,NetAddress(10000));
  server.setConnectionCallBack(onConnection);
  server.setMessageCallBack(onMessage);
  server.start();
  loop.loop();
}


void newConnCallBack(int connfd, const NetAddress &addr)
{
  log_trace("accept new connection from %s",addr.toHostPort().c_str());
  ::write(connfd,"how are you?\n",13);
  ::close(connfd);
}
TEST(testAccept)
{
    EventLoop loop;
    Acceptor acceptor(&loop,NetAddress(10000));
    acceptor.setNewConnectCallBack(newConnCallBack);
    acceptor.listen();
    loop.loop();
}

TEST(testNetAddress)
{
  NetAddress inet(32767);
  std::cout << inet.toHostPort() << std::endl;
}


TEST(testEventLoopThread)
{
  printf("main %lx\n",CurrentThread::tid());
  EventLoopThread loopThread;
  EventLoop *loop = loopThread.startLoop();
  loop->runInLoop([]{
      printf("loop %lx\n",CurrentThread::tid());
  });
  loop->runAfter(3,[]{
      printf("loop %lx\n",CurrentThread::tid());
  });
  sleep(4);
  loop->quit();
}


//测试线程间任务调配
TEST(testSafeTimeCall)
{
  EventLoop loop;
  std::thread t([&]{
    loop.runAfter(1.0,[]{ printf("%lx run\n",CurrentThread::tid());});
    loop.quit();
  });
  loop.loop();
  t.join();
}


int g_flag = 0;

void run4()
{
  printf("run4(): pid = %x, flag = %d\n", getpid(), g_flag);
  globelLoop->quit();
}

void run3()
{
  printf("run3(): pid = %x, flag = %d\n", getpid(), g_flag);
  globelLoop->runAfter(3, run4);
  g_flag = 3;
}

void run2()
{
  printf("run2(): pid = %x, flag = %d\n", getpid(), g_flag);
  globelLoop->queueInloop(run3);
}

void run1()
{
  g_flag = 1;
  printf("run1(): pid = %x, flag = %d\n", getpid(), g_flag);
  globelLoop->runInLoop(run2);
  g_flag = 2;
}

TEST(testQueue)
{
    printf("main(): pid = %x, flag = %d\n", getpid(), g_flag);

    EventLoop loop;
    globelLoop = &loop;

    loop.runAfter(2, run1);
    loop.loop();
    printf("main(): pid = %x, flag = %d\n", getpid(), g_flag);
}


int cnt = 0;
void print(const char* msg)
{
  log_trace("time: %s  : msg %s",TimeStamp::now().toString().c_str(),msg);
  if(cnt++ > 10)
    {
      globelLoop->quit();
    }
}
TEST(testTimer)
{
  EventLoop loop;
  globelLoop = &loop;
  loop.runAfter(1, std::bind(print,"once1"));
  loop.runAfter(1.5, std::bind(print,"once1.5"));
  loop.runAfter(2.5, std::bind(print,"once2.5"));
  loop.runAfter(5.5, std::bind(print,"once5.5"));
  loop.runEvery(2,std::bind(print,"every2"));
  loop.runEvery(3,std::bind(print,"every3"));
  loop.loop();
}
