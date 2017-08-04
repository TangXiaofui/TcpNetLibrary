/*
 * UniqueLock.h
 *
 *  Created on: Jun 30, 2017
 *      Author: txh
 */

#ifndef UNIQUELOCK_H_
#define UNIQUELOCK_H_
#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
using namespace std;

//竞态条件
//析构一个对象，从何得知是否有别的线程在执行该对象的成员函数
//如何保证在执行成员函数期间，对象不会在另一个线程中被析构
//在调用某个对象之前，如何得知该对象是否还或者


//1.不要在构造期间，将this指针传给其他线程


extern int counter;

void func();

class Interge{
public:
  int getValue();

  mutex mtx;
  int value;
};

class Counter{
public:
  Counter();
  //2.无法在析构中使用互斥锁，生命周期
  ~Counter();
  void func();
  //3.同时读写一个Class的两个对象，有发生死锁的可能，如A（a,b）,B(b,a),可以通过在内部判断对象地址来解决
  void Swap(Interge &a,Interge &b);
  //下面同样的道理
  Counter& operator=(const Counter& rh);

  mutex mtx;
  int count;
};

void testMutex();

#endif /* UNIQUELOCK_H_ */
