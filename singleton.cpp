/*
 * singleton.cpp
 *
 *  Created on: Jul 1, 2017
 *      Author: txh
 */

#include "singleton.h"



void testSingleton()
{
  int &a = Singleton<int>::getInstance();
  a = 10;
  int b = Singleton<int>::getInstance();
  cout << b << endl;
}
