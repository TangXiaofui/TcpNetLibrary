/*
 * Observable.cpp
 *
 *  Created on: Jun 30, 2017
 *      Author: txh
 */

#include "Observable.h"
#include <iostream>

void Observer::update()
{
   cout << "get some msg from observable" << endl;
}

Observer::~Observer()
{
  cout << "~Observer" << endl;
}


Observable::Observable()
{
  cout << "Observable" << endl;
}

Observable::~Observable()
{
  cout << "~Observable()" << endl;
}
void Observable::register_(weak_ptr<Observer> ob)
//void Observable::register_(shared_ptr<Observer> ob)
{
  unique_lock<mutex> lock(mtx);
  vObservers.emplace_back(ob);
}

void Observable::notifyAll()
{
  unique_lock<mutex> lock(mtx);
  auto it = vObservers.begin();
  while(it != vObservers.end())
  {
      shared_ptr<Observer> ob = it->lock();
//      shared_ptr<Observer> ob = *it;
      if(ob)
	{
	  ob->update();
	  it++;
	}
      else
	{
	  it = vObservers.erase(it);
	}
  }
}


