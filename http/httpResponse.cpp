/*
 * httpResponse.cpp
 *
 *  Created on: Jul 18, 2017
 *      Author: txh
 */


#include "httpResponse.h"
#include <stdio.h>
HttpResponse::HttpResponse(bool close)
:statusCode_(kUnknow),
 closeConnection_(close)
{

}

void HttpResponse::setStatusCode(HttpResponse::HttpStatusCode code)
{
  statusCode_ = code;
}

void HttpResponse::setStatusMessage(const string &message)
{
  statusMessage_ = message;
}

void HttpResponse::setCloseConnection(bool on)
{
  closeConnection_ = on;
}

bool HttpResponse::closeConnection()
{
  return closeConnection_;
}

void HttpResponse::setContentType(const string& contentType)
{
  addHeader("Content-Type",contentType);
}

void HttpResponse::addHeader(const string& key, const string& value)
{
  headers_[key] = value;
}

void HttpResponse::setBody(const string& body)
{
  body_ = body;
}

void HttpResponse::appendTobuffer(Buffer *outbuf) const
{
  char buf[32];
  snprintf(buf,sizeof buf, "HTTP/1.1 %d ",statusCode_);
  outbuf->append(buf);
  outbuf->append(statusMessage_);
  outbuf->append("\r\n");
  if(closeConnection_)
    {
      outbuf->append("Connection: close\r\n");
    }
  else
    {
      snprintf(buf,sizeof buf , "Content-Length: %zd\r\n",body_.size());
      outbuf->append(buf);
      outbuf->append("Connection: Keep-Alive\r\n");
    }

  for(std::map<string,string>::const_iterator it = headers_.begin(); it != headers_.end() ; it++)
    {
      outbuf->append(it->first);
      outbuf->append(": ");
      outbuf->append(it->second);
      outbuf->append("\r\n");
    }

  outbuf->append("\r\n");
  outbuf->append(body_);


}

