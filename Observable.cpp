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
namespace {
class A{
public:
  A(){
    cout << "a" << endl;
  }
  ~A(){
    cout << "~a" << endl;
  }
};

class B{
public:
  B(shared_ptr<A> a):ta(a)
  {
    cout << "b" << endl;
  }
  ~B()
  {
    cout << "~B" << endl;
  }

  shared_ptr<A> ta;
};

}
void testObserver()
{
  //    Observable ob;
  //    ob.register_(make_shared<Observer>());
  //    cout << "--------------" << endl;
  //    ob.register_(make_shared<Observer>());
  //    ob.notifyAll();
  //    shared_ptr<A> ta = make_shared<A>();
  ////    B b(ta);
  //    ta = make_shared<A>();
  //    cout << "-----------------" << endl;
  //
  //    shared_ptr<A> a(new A[10],[](A *a){delete[] a;});

  //    shared_ptr<StockFactory> f(new StockFactory);
  //    shared_ptr<Stock> s = f->get("test");
  //    f->get("test1");
  //    f->get("test");
  //    cout << "size :" << f->getSize() << endl;
  //    cout << "------------" << endl;

}


