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

//对buffer内的http请求进行解析，将解析后的内容填充到request_
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

  //针对不同的调用，返回不同的对象
  const HttpRequest & request() const;
  HttpRequest& request();

private:
  bool processRequestLine(const char* begin, const char* end);

  HttpRequestParseState state_;
  HttpRequest request_;

};



#endif /* HTTPCONTEXT_H_ */
