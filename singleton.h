
/*
 * singleton.h
 *
 *  Created on: Jul 1, 2017
 *      Author: txh
 */

#ifndef SINGLETON_H_
#define SINGLETON_H_

#include <pthread.h>
#include <iostream>
#include <stdlib.h>
using namespace std;

template<typename T>
class Singleton{
public:
  static T& getInstance()
  {
    pthread_once(&ponc_,&Singleton::init);
    return *value;
  }

private:
  Singleton()
  {
    cout << "Singleton" << endl;
  }
  ~Singleton()
  {
    cout << "~Singleton" << endl;
  }
  static void init()
  {
    value = new T();
    atexit(&Singleton::destroy);
    cout << "init" << endl;
  }
  static void destroy(){
    if(value)
      delete value;
    cout << "destroy" << endl;
  }
  static pthread_once_t ponc_;
  static T* value;
};

template<typename T>
pthread_once_t Singleton<T>::ponc_ = PTHREAD_ONCE_INIT;

template<typename T>
T* Singleton<T>::value = nullptr;

void testSingleton();

#endif /* SINGLETON_H_ */
