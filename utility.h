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

using TimerCallBack = std::function<void()>;
using Functor = std::function<void()>;
class TcpConnection;
using TcpConnectionPtr = std::shared_ptr<TcpConnection>;
using ConnectionCallBack = std::function<void (const TcpConnectionPtr&)>;
using MessageCallBack = std::function<void (const TcpConnectionPtr&,const char* data,ssize_t len)>;

#endif /* UTILITY_H_ */
