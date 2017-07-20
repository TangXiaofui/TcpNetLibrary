/*
 * httpContext.cpp
 *
 *  Created on: Jul 18, 2017
 *      Author: txh
 */


#include "httpContext.h"
#include <algorithm>

HttpContext::HttpContext()
:state_(kExpectRequestLine)
{

}

bool HttpContext::gotAll() const
{
  return state_ == kGotAll;
}

void HttpContext::reset()
{
  state_ = kExpectRequestLine;

  HttpRequest dummy;
  request_.swap(dummy);
}

const HttpRequest& HttpContext::request() const
{
  return request_;
}

HttpRequest& HttpContext::request()
{
  return request_;
}

bool HttpContext::parseRequest(Buffer* buf, TimeStamp receiveTime)
{
  bool ok = true;
  bool hasMore = true;
  //http协议请求行，请求头，请求体都是按照\r\n分割开
  while(hasMore)
    {
      //状态机，依次处理各个段
      if(state_ == kExpectRequestLine)
	{
	  const char *crlf = buf->findCRLF();
	  if(crlf)
	    {
	      ok = processRequestLine(buf->peek(),crlf);
	      if(ok)
		{
		  request_.setReceiveTime(receiveTime);
		  buf->retrieveUntil(crlf+2);
		  state_ = kExpectRequestHead;
		}
	      else
		{
		  hasMore = false;
		}
	    }
	  else
	    {
	      hasMore = false;
	    }
	}
      else if(state_ == kExpectRequestHead)
	{
	    const char* crlf = buf->findCRLF();
	    if(crlf)
	      {
		const char *colon = std::find(buf->peek(),crlf,':');
		if(colon != crlf)
		  {
		    request_.addHeader(buf->peek(),colon,crlf);
		  }
		else
		  {
		    state_ = kGotAll;
		    hasMore = false;
		  }
		buf->retrieveUntil(crlf+2);
	      }
	    else
	      {
		hasMore = false;
	      }
	}
      else if(state_ == kExpectRequestBody)
	{

	}
    }
  return ok;
}

bool HttpContext::processRequestLine(const char* begin, const char* end)
{
  bool success = true;
  const char *start = begin;
  const char *space = std::find(start,end,' ');
  //解析请求行，以空格为分割
  if(space != end && request_.setMethod(begin,space))
    {
	start = space + 1;
	space = std::find(start,end,' ');
	if(space != end)
	  {
	    //url中是否有其他请求
	    const char *question = std::find(start,space,'?');
	    if(question != space)
	      {
		request_.setPath(start,question);
		request_.setQuery(question,space);
	      }
	    else
	      {
		request_.setPath(start,space);
	      }
	  }
	start = space + 1;
	success = (end - start) == 8 && std::equal(start,end-1,"HTTP/1.");
	if(success)
	  {
	    if(*(end-1) == '1')
	      request_.setVersion(HttpRequest::kHttp11);
	    else if(*(end - 1) == '0')
	      request_.setVersion(HttpRequest::kHttp10);
	    else
	      success = false;
	  }
    }
  return success;
}

