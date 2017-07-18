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

  void setStatusCode(HttpStatusCode code);
  void setStatusMessage(const string &message);

  void setCloseConnection(bool on);

  bool closeConnection();

  void setContentType(const string& contentType);

  void addHeader(const string& key, const string& value);

  void setBody(const string& body);

  void appendTobuffer(Buffer *outbuf) const;

private:
  std::map<string,string> headers_;
  HttpStatusCode statusCode_;
  string statusMessage_;
  bool closeConnection_;
  string body_;
};


#endif /* HTTPRESPONSE_H_ */
