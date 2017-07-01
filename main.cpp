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
#include "singleton.h"
#include <cassert>


void testShared()
{
  shared_ptr<int> a(new int(5));
  shared_ptr<int> b = a;
  {
    shared_ptr<int> c(new int(10));
    a.swap(c);
  }
  cout << *a << " " << *b << endl;
}

int main(void) {

//    testObjectLife();
    testSingleton();

    return EXIT_SUCCESS;
}
