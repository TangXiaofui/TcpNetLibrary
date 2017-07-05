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


int main(void) {

    EventLoop loop;
    int timefd = ::timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK|TFD_CLOEXEC);
    Channel channel(&loop,timefd);
    channel.setReadCallBack([&](){
      info("timeout");
      loop.quit();
    });
    channel.enableReading();
    struct itimerspec howlong;
    ::bzero(&howlong,sizeof(howlong));
    howlong.it_value.tv_sec = 5;
    ::timerfd_settime(timefd,0,&howlong,NULL);
    loop.loop();
    ::close(timefd);

    return EXIT_SUCCESS;
}
