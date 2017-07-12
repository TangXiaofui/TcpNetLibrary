/*
 * timerId.cpp
 *
 *  Created on: Jul 6, 2017
 *      Author: txh
 */

#include "timerId.h"


TimerId::TimerId(Timer *timer,int64_t seq)
:timer_(timer),
sequence_(seq)
{

}
