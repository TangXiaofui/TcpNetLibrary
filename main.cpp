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
#include <assert.h>

//#include "UniqueLock.h"
//#include "Observable.h"
//#include "StockFactory.h"
//#include "singleton.h"
//#include "pthreadKey.h"
#include "utility.h"
#include "logging.h"
#include "eventLoop.h"
#include "unitTest.h"
#include "eventLoopThread.h"
#include "netAddress.h"
#include "acceptor.h"
#include "tcpServer.h"
#include "ignoreSigPipe.h"
#include "eventLoopThreadPool.h"
#include "connector.h"
#include "http/httpContext.h"
#include "http/httpServer_test.h"


IgnoreSigPipe initObj;


/*netlibrary，主要是框架设计（Reactor+io复用+线程池），优化策略（建立索引map，减小临界区,智能指针，RAII,线程间的通讯选择），可维护性(模块复用)，
*		可扩展性（模块化设计，降低耦合），可靠性（容错，日志系统，代码覆盖率,错误处理(assert断言),线程安全），代码逻辑清晰（注释)
*		使用调试工具进行各种测试，Valgrind strace  lsof tcpdump
*		时刻注意现在处于的线程中，以及尽量将锁的粒度降低
*/
EventLoop *globelLoop;


int main(void) {


   RunAllTests("testTcpServer");

   return EXIT_SUCCESS;
}

TEST(testHttp)
{
  testHttpServer();
}

TEST(testHttpRequest)
{
  Buffer input;
  input.append("GET /index.html HTTP/1.1\r\n"
      "Host:www.test.com\r\n"
      "\r\n");

  HttpContext context;
  assert(context.parseRequest(&input,TimeStamp::now()));

  const HttpRequest& request = context.request();
  assert(request.getMethod() == HttpRequest::kGet);
  assert(request.getPath() == string("/index.html"));
  assert(request.getVersion() == HttpRequest::kHttp11);
  assert(request.getHeader("Host") == string("www.test.com"));
  assert(request.getHeader("User-Agent") == string(""));

}

void connectionCallback(int sockfd)
{
  printf("connected.\n");
  globelLoop->quit();

}

TEST(testConnector)
{
  EventLoop loop;
  globelLoop = &loop;
  Connector conn(&loop,NetAddress("127.0.0.1",10000));
  conn.setNewConnectionCallBack(connectionCallback);
  conn.start();
  loop.loop();
}

void onConnection(const TcpConnectionPtr& conn)
{
  if(conn->isConneted())
    {
      conn->setTcpNoDelay(true);
//      log_info("new Connection tid:%lx %s from %s",CurrentThread::tid(),conn->getName().c_str(),conn->getPeerAddr().toHostPort().c_str());
    }
   else
    {
//      log_info("connection tid:%lx %s is down",CurrentThread::tid(),conn->getName().c_str());
    }

}
void onMessage(const TcpConnectionPtr& conn,Buffer *buf,TimeStamp when)
{
//  log_info("onMessage tid:%lx receive msg(%zd) :  from peer %s",CurrentThread::tid(),buf->readableBytes(),conn->getPeerAddr().toHostPort().c_str());
  conn->send(buf->retrieveAsString());
}

TEST(testTcpServer)
{
  Logger::getLogger().setLogLevel(Logger::LogLevel::ERROR);
//  Logger::getLogger().setFileName("log");
  printf("main():pid = %lx\n",CurrentThread::tid());
  EventLoop loop;
  TcpServer server(&loop,NetAddress(10000));
  server.setConnectionCallBack(onConnection);
  server.setMessageCallBack(onMessage);
//  server.setThreadNum(3);
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

TimerId toCancel;
void cancelSelf()
{
  print("cancel self\n");
  globelLoop->cancel(toCancel);
}

TEST(testTimer)
{
  EventLoop loop;
  globelLoop = &loop;
//  loop.runAfter(1, std::bind(print,"once1"));
//  loop.runAfter(1.5, std::bind(print,"once1.5"));
//  loop.runAfter(2.5, std::bind(print,"once2.5"));
//  loop.runAfter(5.5, std::bind(print,"once5.5"));
//  loop.runEvery(2,std::bind(print,"every2"));
//  loop.runEvery(3,std::bind(print,"every3"));
  toCancel = loop.runEvery(1,cancelSelf);
  loop.loop();
}

TEST(testLog)
{
  Logger::getLogger().setLogLevel(Logger::LogLevel::TRACE);
  Logger::getLogger().setFileName("log");
  int num = 100;
  for(int i = 0 ; i < num;i++)
    {
      log_error("testlog");
    }
  log_error("i = %d",num);
}
