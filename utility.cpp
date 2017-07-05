/*
 * utility.cpp
 *
 *  Created on: Jul 5, 2017
 *      Author: txh
 */


#include "utility.h"
#include <sys/syscall.h>
#include <unistd.h>

long CurrentThread::tid()
{
  return syscall(SYS_gettid);
}
