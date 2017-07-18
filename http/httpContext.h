/*
 * httpContext.h
 *
 *  Created on: Jul 18, 2017
 *      Author: txh
 */

#ifndef HTTPCONTEXT_H_
#define HTTPCONTEXT_H_

#include "../utility.h"
#include "httpRequest.h"
#include "../buffer.h"
#include "../timeStamp.h"

class HttpContext:public copyable{
public:
  enum HttpRequestParseState{
    kExpectRequestLine,
    kExpectRequestHead,
    kExpectRequestBody,
    kGotAll
  };
  HttpContext();
  bool parseRequest(Buffer* buf, TimeStamp receiveTime);

  bool gotAll() const;

  void reset();

  const HttpRequest & request() const;
  HttpRequest& request();

private:
  bool processRequestLine(const char* begin, const char* end);

  HttpRequestParseState state_;
  HttpRequest request_;

};



#endif /* HTTPCONTEXT_H_ */
