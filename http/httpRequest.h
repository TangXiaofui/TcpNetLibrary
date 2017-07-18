/*
 * httpRequest.h
 *
 *  Created on: Jul 18, 2017
 *      Author: txh
 */

#ifndef HTTPREQUEST_H_
#define HTTPREQUEST_H_

#include "../utility.h"
#include "../timeStamp.h"
#include <string>
#include <map>

class HttpRequest:public copyable{
public:
  enum Method{
    kInvaild,
    kGet,
    kPost,
    kHead,
    kPut,
    kDelete
  };
  enum Version{
    kUnknow,
    kHttp10,
    kHttp11
  };

  HttpRequest();

  void setVersion(Version version);
  Version getVersion() const;

  bool setMethod(const char *start,const char *end);
  Method getMethod() const;

  const char* methodToString() const;
  void setPath(const char* start, const char *end);
  const string& getPath() const;

  void setQuery(const char *start, const char *end);
  const string& getQuery() const;

  void setReceiveTime(TimeStamp t);
  TimeStamp getReceiveTime() const;

  void addHeader(const char *start, const char* colon, const char* end);
  string getHeader(const string& field) const;

  const std::map<string,string>& headers() const;

  void swap(HttpRequest &that);

private:
  Method method_;
  Version version_;
  string path_;
  string query_;
  TimeStamp receiveTime_;
  std::map<string,string> headers_;


};



#endif /* HTTPREQUEST_H_ */
