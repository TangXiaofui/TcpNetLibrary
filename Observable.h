/*
 * Observable.h
 *
 *  Created on: Jun 30, 2017
 *      Author: txh
 */

#ifndef OBSERVABLE_H_
#define OBSERVABLE_H_

#include <memory>
#include <iostream>
#include <mutex>
#include <vector>
using namespace std;

class Observer{
public:
  void update();
  ~Observer();
};

class Observable{
public:
  Observable();
  ~Observable();
  void register_(weak_ptr<Observer> ob);
//  void register_(shared_ptr<Observer> ob);
  void notifyAll();

private:
  vector<weak_ptr<Observer> > vObservers;
//  vector<shared_ptr<Observer> > vObservers;
  mutex mtx;
};





#endif /* OBSERVABLE_H_ */
