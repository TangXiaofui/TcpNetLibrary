/*
 * timerId.h
 *
 *  Created on: Jul 6, 2017
 *      Author: txh
 */

#ifndef TIMERID_H_
#define TIMERID_H_

#include "utility.h"

class Timer;

class TimerId:public copyable{
public:
  TimerId(Timer *timer = nullptr,int64_t seq = 0);


  friend class TimerQueue;
private:
  Timer* timer_;
  int64_t sequence_;
};



#endif /* TIMERID_H_ */
