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


void testMutex()
{
      std::vector<thread> worker;
      for(int i = 0 ; i < 20; i++)
        {
  	worker.emplace_back(func);
        }
      for(auto &t : worker)
        {
  	t.join();
        }
      cout << counter << endl;
}
