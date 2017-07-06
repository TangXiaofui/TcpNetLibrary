/*
 * timeStamp.h
 *
 *  Created on: Jul 6, 2017
 *      Author: txh
 */

#ifndef TIMESTAMP_H_
#define TIMESTAMP_H_

#include "utility.h"
#include <string>

class TimeStamp:public copyable{
public:
  TimeStamp();
  explicit TimeStamp(int64_t microSecondsSinceEpoch);
  static TimeStamp now();
  int64_t microSecondsSinceEpoch();
  std::string toString() const;

  bool valid();
  static TimeStamp invaild();
  static const int kMicroSecondsPerSecond = 1000 * 1000;
private:

  int64_t microSecondsSinceEpoch_;

};


inline TimeStamp addTime(TimeStamp now,double second)
{
  int64_t delta = static_cast<int64_t>(second * TimeStamp::kMicroSecondsPerSecond);
  return TimeStamp(now.microSecondsSinceEpoch()+delta);
}

inline bool operator < (TimeStamp lhs,TimeStamp rhs)
{
  return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline bool operator == (TimeStamp lhs, TimeStamp rhs)
{
  return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

inline double timeDifference(TimeStamp high, TimeStamp low)
{
  int64_t diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
  return static_cast<double>(diff) / TimeStamp::kMicroSecondsPerSecond;
}

#endif /* TIMESTAMP_H_ */
