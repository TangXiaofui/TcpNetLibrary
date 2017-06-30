/*
 * StockFactory.cpp
 *
 *  Created on: Jun 30, 2017
 *      Author: txh
 */
#include "StockFactory.h"
#include <iostream>
using namespace std;

Stock::Stock(string key)
:key_(key)
{
  cout << "Sock " << key <<endl;
}

Stock::~Stock()
{
  cout << "~Sock " << endl;
}
string Stock::key()
{
  return key_;
}

shared_ptr<Stock> StockFactory::get(const string& key)
{
  shared_ptr<Stock> ret;
  unique_lock<mutex> lock(mtx);
  weak_ptr<Stock>& wp = stocks_[key];
  ret = wp.lock();
  if(!ret){
      ret.reset(new Stock(key),[this](Stock *stock){this->deleteStock(stock);});	//防止内存泄漏
      wp = ret;
  }
  return ret;
}
size_t StockFactory::getSize()
{
  return stocks_.size();
}
void StockFactory::deleteStock(Stock *stock)
{
  if(stock)
    {
      unique_lock<mutex> lock(mtx);
      stocks_.erase(stock->key());
    }
  delete stock;
}
