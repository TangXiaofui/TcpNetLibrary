/*
 * UniqueLock.cpp
 *
 *  Created on: Jun 30, 2017
 *      Author: txh
 */

#include "UniqueLock.h"


mutex mtx;
int counter = 0;
void func()
{
  unique_lock<mutex> lock(mutex);
//  this_thread::sleep_for(std::chrono::seconds(1));
  counter++;

}

