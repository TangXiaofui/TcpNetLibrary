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
#include <sys/timerfd.h>
#include <string.h>
#include <memory>
#include <set>

int cnt = 0;
EventLoop *globelLoop;

void print(const char* msg)
{
  trace("time: %s  : msg %s",TimeStamp::now().toString().c_str(),msg);
  if(cnt++ > 20)
    {
      globelLoop->quit();
    }
}

int main(void) {

    EventLoop loop;
    globelLoop = &loop;
    loop.runAfter(1, std::bind(print,"once1"));
    loop.runAfter(1.5, std::bind(print,"once1.5"));
    loop.runAfter(2.5, std::bind(print,"once2.5"));
    loop.runAfter(5.5, std::bind(print,"once5.5"));
    loop.runEvery(2,std::bind(print,"every2"));
    loop.runEvery(3,std::bind(print,"every3"));

    loop.loop();
   return EXIT_SUCCESS;
}
