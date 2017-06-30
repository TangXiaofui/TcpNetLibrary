//============================================================================
// Name        : MultiThreadSafe.cpp
// Author      : txh
// Version     :
// Copyright   : 
// Description : Hello World in C, Ansi-style
//============================================================================

#include "UniqueLock.h"
#include "Observable.h"
#include "StockFactory.h"


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


int main(void) {

//    std::vector<thread> worker;
//    for(int i = 0 ; i < 20; i++)
//      {
//	worker.emplace_back(func);
//      }
//    for(auto &t : worker)
//      {
//	t.join();
//      }
//    cout << counter << endl;

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

    StockFactory f;
    shared_ptr<Stock> s = f.get("test");
    f.get("test1");
    f.get("test");
    cout << "size :" << f.getSize() << endl;
    cout << "------------" << endl;

    return EXIT_SUCCESS;
}
