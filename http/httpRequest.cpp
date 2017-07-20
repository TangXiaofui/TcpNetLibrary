/*
 * httpRequest.cpp
 *
 *  Created on: Jul 18, 2017
 *      Author: txh
 */

#include "httpRequest.h"

HttpRequest::HttpRequest()
:method_(kInvaild),
 version_(kUnknow)
{

}

void HttpRequest::setVersion(Version version)
{
  version_ = version;
}

HttpRequest::Version HttpRequest::getVersion() const
{
  return version_;
}

bool HttpRequest::setMethod(const char *start,const char *end)
{
  string m(start,end);
  if(m == "GET")
    method_ = kGet;
  else if(m == "POST")
    method_ = kPost;
  else if(m == "HEAD")
    method_ = kHead;
  else if(m == "PUT")
    method_ = kPut;
  else if(m == "DELETE")
    method_ = kDelete;
  else
    method_ = kInvaild;

  return method_ != kInvaild;

}

HttpRequest::Method HttpRequest::getMethod() const
{
  return method_;
}

const char* HttpRequest::methodToString() const
{
  const char * res = "unknow";
  switch(method_){
    case kGet:
	res = "GET";
      break;
    case kPost:
	res = "POST";
	break;
    case kHead:
        res = "HEAD";
        break;
    case kPut:
        res = "PUT";
        break;
    case kDelete:
	 res = "DELETE";
	 break;
    default:
         break;
  }
  return res;
}

void HttpRequest::setPath(const char* start, const char *end)
{
  path_.assign(start,end);
}

const string& HttpRequest::getPath() const
{
  return path_;
}


void HttpRequest::setQuery(const char *start, const char *end)
{
  query_.assign(start,end);
}

const string& HttpRequest::getQuery() const
{
  return query_;
}

void HttpRequest::setReceiveTime(TimeStamp t)
{
  receiveTime_ = t;
}

TimeStamp HttpRequest::getReceiveTime() const
{
  return receiveTime_;
}

void HttpRequest::addHeader(const char *start, const char* colon, const char* end)
{
  string field(start,colon);
  ++colon;
  // 去除前后空格
  while(colon < end && isspace(*colon)){
      ++colon;
  }
  string value(colon, end);
  while(!value.empty() && isspace(value[value.size() - 1 ] )){
      value.resize(value.size() - 1 );
  }

  headers_[field] = value;
}

string HttpRequest::getHeader(const string& field) const
{
  string result;
  std::map<string,string>::const_iterator it = headers_.find(field);
  if(it != headers_.end())
    {
      result = it->second;
    }
  return result;
}

const std::map<string,string>& HttpRequest::headers() const
{
  return headers_;
}

void HttpRequest::swap(HttpRequest &that)
{
  std::swap(method_,that.method_);
  std::swap(version_,that.version_);
  path_.swap(that.path_);
  query_.swap(that.query_);
  receiveTime_.swap(that.receiveTime_);
  headers_.swap(that.headers_);

}


