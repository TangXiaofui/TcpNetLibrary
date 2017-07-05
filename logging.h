/*
 * logging.h
 *
 *  Created on: Jul 5, 2017
 *      Author: txh
 */

#ifndef LOGGING_H_
#define LOGGING_H_

#include "utility.h"
#include <string>

#define hlog(level,...)  	\
do{				\
    if(level >= Logger::getLogger().getLogLevel()){	\
	Logger::getLogger().record(level,__FILE__,__LINE__,__func__,__VA_ARGS__); \
     }							\
}while(0)

#define trace(...) hlog(Logger::LogLevel::TRACE,__VA_ARGS__)
#define debug(...) hlog(Logger::LogLevel::DEBUG,__VA_ARGS__)
#define info(...)  hlog(Logger::LogLevel::INFO,__VA_ARGS__)
#define warn(...)  hlog(Logger::LogLevel::WARN,__VA_ARGS__)
#define error(...) hlog(Logger::LogLevel::ERROR,__VA_ARGS__)
#define fatal(...) do { hlog(Logger::LogLevel::FATAL,__VA_ARGS__); exit(1);} while(0)



class Logger:public noncopyable{
public:
  //这里不能使用enum class
  enum LogLevel{
    TRACE = 0,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
    ALL
  };

  //多线程条件下可能需要加锁
  void record(int level,const char* file,int line,const char* funcName,const char*fmt ...);

  void setLogLevel(LogLevel level);
  void setRotateInterval(long rotateInterval);
  void setFileName(const std::string& fileName = std::string("log"));
  LogLevel getLogLevel();
  static Logger& getLogger();

private:
  Logger();
  ~Logger();
  LogLevel level_;
  int fd_;
  long rotateInterval_;
  long lastRotate_;
  std::string fileName_;
  static const char* levelStr[ALL+1];
  void RotateFile();

};



#endif /* LOGGING_H_ */
