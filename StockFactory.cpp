/*
 * StockFactory.cpp
 *
 *  Created on: Jun 30, 2017
 *      Author: txh
 */
#include "StockFactory.h"
#include <iostream>
#include <functional>
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
  ;
  if(!ret){
//      ret.reset(new Stock(key),[&](Stock *stock){shared_from_this()->deleteStock(stock);});	//防止内存泄漏
      ret.reset(new Stock(key),std::bind(&StockFactory::weakDeleteStock,
                                         weak_ptr<StockFactory>(shared_from_this()),
                                                                std::placeholders::_1));
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
void StockFactory::weakDeleteStock(const weak_ptr<StockFactory> &wp,Stock *stock)
{
  shared_ptr<StockFactory> sf(wp.lock());
  if(sf)
    {
      sf->removeStock(stock);
    }
  delete stock;
}
void StockFactory::removeStock(Stock* stock)
{
  if(stock)
      {
        unique_lock<mutex> lock(mtx);
        stocks_.erase(stock->key());
      }
}

StockFactory::~StockFactory()
{
  cout << "~StockFactory" << endl;
}


void testObjectLife()
{
  //    shared_ptr<StockFactory> f(new StockFactory);
  //    {
  //      shared_ptr<Stock> s = f->get("test");
  //      shared_ptr<Stock> s1 = f->get("test1");
  //    }
  //    cout << "size :" << f->getSize() << endl;

      shared_ptr<Stock> s;
      {
        shared_ptr<StockFactory> f(new StockFactory);
        s = f->get("test");
        shared_ptr<Stock> s1 = f->get("test1");

      }
}
