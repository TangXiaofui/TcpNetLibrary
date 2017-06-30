/*
 * StockFactory.h
 *
 *  Created on: Jun 30, 2017
 *      Author: txh
 */

#ifndef STOCKFACTORY_H_
#define STOCKFACTORY_H_

#include <memory>
#include <map>
#include <mutex>
#include <string>
using namespace std;

class Stock{
public:
  Stock(string key);
  ~Stock();
  string key();
  string key_;
};

class StockFactory{
public:
  shared_ptr<Stock> get(const string &key);
  size_t getSize();
  void deleteStock(Stock *stock);
private:
  map<string,weak_ptr<Stock>> stocks_;
  mutex mtx;
};



#endif /* STOCKFACTORY_H_ */
