/*
 * httpServer_test.cpp
 *
 *  Created on: Jul 18, 2017
 *      Author: txh
 */

#include "httpServer_test.h"
#include "httpRequest.h"
#include "httpResponse.h"
#include "httpServer.h"
bool benchmark = false;


void onRequest(const HttpRequest & req, HttpResponse *resp)
{
  log_info("Headers %s , Path %s ",req.methodToString(), req.getPath().c_str());
  if(!benchmark)
    {
      const std::map<string,string> &headers = req.headers();
      for(std::map<string,string>::const_iterator it = headers.begin(); it != headers.end(); it++)
	{
	  log_info("%s : %s",it->first.c_str(),it->second.c_str());
	}
    }

  if(req.getPath() == "/")
    {
      resp->setStatusCode(HttpResponse::k200OK);
      resp->setStatusMessage("OK");
      resp->setContentType("text/html");
      resp->addHeader("server","txh");
      string now = TimeStamp::now().toFormattedString();
      resp->setBody("<html><head><title>this is title</title></head>"
	  "<body><h1>hello</h1>Now is "+now+
	  "</body></html>");
    }
}

void testHttpServer( int nums )
{
  int numThread = 0;
  if(nums > 0)
    {
      benchmark = true;
      numThread = 0;
    }

   EventLoop loop;
   HttpServer server(&loop,NetAddress(8000),"server");
   server.setHttpCallback(onRequest);
   server.setThreadNum(numThread);
   server.start();
   loop.loop();
}



