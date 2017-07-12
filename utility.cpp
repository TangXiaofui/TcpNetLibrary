/*
 * utility.cpp
 *
 *  Created on: Jul 5, 2017
 *      Author: txh
 */


#include "utility.h"
#include "logging.h"

#include <sys/syscall.h>
#include <unistd.h>
#include <string.h>



long CurrentThread::tid()
{
  return syscall(SYS_gettid);
}

__thread char t_errnobuf[512];
const char* strerror_tl(int savedErrno)
{
  return strerror_r(savedErrno, t_errnobuf, sizeof t_errnobuf);
}
