/*
 * httpResponse.h
 *
 *  Created on: Jul 18, 2017
 *      Author: txh
 */

#ifndef HTTPRESPONSE_H_
#define HTTPRESPONSE_H_


#include "../utility.h"
#include "../buffer.h"
#include <string>
#include <map>
using namespace std;


//设置完相应属性后，将httpResponse组成成buffer对象返回给用户
class HttpResponse:public copyable{
public:
  enum HttpStatusCode{
    kUnknow,
    k200OK = 200,
    k301MovePermently = 301,
    k400BadRequest = 400,
    k404NotFound = 404,
  };

  explicit HttpResponse(bool close);

  //设置响应行状态码
  void setStatusCode(HttpStatusCode code);
  //设置响应行状态信息
  void setStatusMessage(const string &message);

  void setCloseConnection(bool on);

  bool closeConnection();

  void setContentType(const string& contentType);

  //设置响应头信息
  void addHeader(const string& key, const string& value);

  //设置响应梯信息
  void setBody(const string& body);

  //组装成buffer
  void appendTobuffer(Buffer *outbuf) const;

private:
  std::map<string,string> headers_;
  HttpStatusCode statusCode_;
  string statusMessage_;
  bool closeConnection_;
  string body_;
};


#endif /* HTTPRESPONSE_H_ */
