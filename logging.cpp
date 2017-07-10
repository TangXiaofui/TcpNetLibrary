/*
 * logging.cpp
 *
 *  Created on: Jul 5, 2017
 *      Author: txh
 */

#include "logging.h"
#include <time.h>
#include <algorithm>
#include <unistd.h>
#include <thread>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/time.h>
#include <stdarg.h>
#include <algorithm>

Logger::Logger():
level_(LogLevel::TRACE),
fd_(-1),
rotateInterval_(86400),
lastRotate_(time(nullptr))
{
  tzset();
}

Logger::~Logger()
{
  if(fd_ != -1)
    {
      ::close(fd_);
    }
}

void Logger::setLogLevel(LogLevel level)
{
  level_ = std::min(level,LogLevel::ALL);
}


void Logger::setRotateInterval(long rotateInterval)
{
  rotateInterval_ = rotateInterval;
}

Logger::LogLevel Logger::getLogLevel()
{
  return level_;
}

Logger& Logger::getLogger()
{
  static Logger logger;
  return logger;
}
const char* Logger::levelStr[LogLevel::ALL+1] = {
    "TRACE",
    "DEBUG",
    "INFO",
    "WRAN",
    "ERROR",
    "FATAL",
    "ALL"
};

__thread long tid = 0;
void Logger::record(int level,const char* file,int line,const char* funcName,const char*fmt ...)
{
  if(tid == 0)
    {
      tid = CurrentThread::tid();
    }
  RotateFile();
  char buf[1024];
  char *pb = buf;
  char *pe = buf + sizeof(buf);

  struct timeval now_tv;
  gettimeofday(&now_tv,NULL);
  const time_t seconds = now_tv.tv_sec;
  struct tm t;
  localtime_r(&seconds,&t);
  //打印tid的id或内存的地址需要统一，如%ld，或%lx
  pb += snprintf(pb,pe-pb,
  "%04d/%02d/%02d-%02d:%02d:%02d:%06d %lx %s %s:%d  ",
  t.tm_year+1900,
  t.tm_mon+1,
  t.tm_mday,
  t.tm_hour,
  t.tm_min,
  t.tm_sec,
  static_cast<int>(now_tv.tv_usec),
  (long)tid,
  levelStr[level],
  file,
  line);

  va_list ap;
  va_start(ap,fmt);
  pb += vsnprintf(pb,pe-pb,fmt,ap);
  va_end(ap);
  pb = std::min(pb,pe-2);
  while(*--pb == '\n')
  {
  }
  *++pb = '\n';
  *++pb = '\0';
  int fd = fd_ == -1 ? 1: fd_;
  int err = write(fd,buf,pb-buf);
  if(err != pb-buf)
  {
      fprintf(stderr,"write file fail %s %s %d",__FILE__,__func__,__LINE__);
      return ;
  }


}

void Logger::RotateFile()
{
  time_t nowTime = time(nullptr);
  if(fileName_.size() == 0 || (nowTime - timezone)/rotateInterval_ == (lastRotate_ - timezone)/rotateInterval_)
    return;

  if( (nowTime - timezone)/rotateInterval_ == (lastRotate_ - timezone)/rotateInterval_)
    return ;
  lastRotate_ = nowTime;
  struct tm ntm;
  localtime_r(&nowTime,&ntm);
  char newName[1024];
  memset(newName,0,sizeof(newName));
  snprintf(newName,sizeof(newName),"%s.%d%02d%02d%02d%02d",
           fileName_.c_str(),ntm.tm_year+1900,ntm.tm_mon+1,ntm.tm_mday,ntm.tm_hour,ntm.tm_min);

  const char* oldName = fileName_.c_str();
  int err = rename(oldName,newName);
  if(err != 0)
  {
      fprintf(stderr,"rename fail, %s %s %d\n",__FILE__,__func__,__LINE__);
      return ;
  }
  int fd = open(fileName_.c_str(),O_APPEND|O_CREAT | O_WRONLY | O_CLOEXEC,DEFFILEMODE);
  if(fd < 0)
  {
      fprintf(stderr,"open %s failed, %s %d\n",__FILE__,__func__,__LINE__);
      return ;
  }

  dup2(fd,fd_);
  close(fd);

}

void Logger::setFileName(const std::string& fileName)
{
  if(fileName.size() != 0)
    fileName_ = fileName;
  int fd = open(fileName_.c_str(),O_APPEND | O_CREAT | O_WRONLY | O_CLOEXEC,DEFFILEMODE);
  if(fd < 0)
    {
      fprintf(stderr,"open %s failed, %s  %d\n",__FILE__,__func__,__LINE__);
    }
  if(fd_ == -1)
    {
      fd_ = fd;
    }
  else
    {
      int r = dup2(fd,fd_);
      if(r < 0)
	fprintf(stderr,"dup2 failed, %s %s %d\n",__FILE__,__func__,__LINE__);
      ::close(fd);
    }
}
