/*
 * utility.h
 *
 *  Created on: Jul 5, 2017
 *      Author: txh
 */

#ifndef UTILITY_H_
#define UTILITY_H_

#include <memory>



using namespace std;
class copyable{
protected:
  copyable() = default;
  ~copyable() = default;
};

class noncopyable{
protected:
  noncopyable() = default;
  ~noncopyable() = default;
  noncopyable(const noncopyable&) = delete;
  noncopyable& operator=(const noncopyable&) = delete;
};

template <typename TO,typename FROM>
inline TO implicit_cast(FROM const &f)
{
  return f;
}

namespace CurrentThread{
  long tid();
}


const char* strerror_tl(int savedErrno);


using TimerCallBack = std::function<void()>;
using Functor = std::function<void()>;
class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using ConnectionCallBack = std::function<void (const TcpConnectionPtr&)>;

class TimeStamp;
class Buffer;
using MessageCallBack = std::function<void (const TcpConnectionPtr&, Buffer*, TimeStamp)>;
using CloseCallBack = std::function<void (const TcpConnectionPtr&)>;

#endif /* UTILITY_H_ */
