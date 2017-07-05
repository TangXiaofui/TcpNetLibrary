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



#endif /* UTILITY_H_ */
