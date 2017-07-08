/*
 * ignoreSigPipe.h
 *
 *  Created on: Jul 8, 2017
 *      Author: txh
 */

#ifndef IGNORESIGPIPE_H_
#define IGNORESIGPIPE_H_

#include <signal.h>

class IgnoreSigPipe{
public:
  IgnoreSigPipe()
  {
    ::signal(SIGPIPE,SIG_IGN);
  }
};



#endif /* IGNORESIGPIPE_H_ */
