//============================================================================
// Name        : MultiThreadSafe.cpp
// Author      : txh
// Version     :
// Copyright   : 
// Description : Hello World in C, Ansi-style
//============================================================================

#include "UniqueLock.h"
#include "Observable.h"
#include "StockFactory.h"
#include "singleton.h"
#include "pthreadKey.h"
#include <cassert>
#include "utility.h"
#include <sys/syscall.h>
#include <typeinfo>	//typeid
#include "logging.h"
#include "eventLoop.h"
#include "channel.h"
#include "unitTest.h"
#include "eventLoopThread.h"
#include <sys/timerfd.h>
#include <string.h>
#include <memory>
#include <set>

/*netlibrary，主要是框架设计（Reactor+io复用+线程池），优化策略（map，减小临界区,智能指针，RAII），可维护性，
*		可扩展性（模块化设计，降低耦合），可靠性（容错，日志系统，代码覆盖率），代码逻辑清晰（注释)
*
*/
EventLoop *globelLoop;


int main(void) {


   RunAllTests("testEventLoopThread");

   return EXIT_SUCCESS;
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
    loop.runAfter(1.0,[]{ printf("%x run\n",CurrentThread::tid());});
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
  trace("time: %s  : msg %s",TimeStamp::now().toString().c_str(),msg);
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
