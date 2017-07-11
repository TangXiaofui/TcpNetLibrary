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
  TimerId(Timer *timer = nullptr);


private:
  int64_t sequence_;
  Timer* timer_;
};



#endif /* TIMERID_H_ */
