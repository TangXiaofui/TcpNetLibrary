/*
 * buffer.h
 *
 *  Created on: Jul 8, 2017
 *      Author: txh
 */

#ifndef BUFFER_H_
#define BUFFER_H_


#include "utility.h"

#include <vector>
#include <string>


class Buffer:public copyable{
public:
  static const size_t kCheapPrepared = 8;
  static const size_t kInitialSize = 1024;

  Buffer();

  void swap(Buffer& rhs);

  size_t readableBytes() const;
  size_t writableBytes() const;
  size_t prependableBytes() const;

  const char*peek() const;
  void retrieve(size_t len);
  void retrieveUntil(const char* end);
  void retrieveAll();
  std::string retrieveAsString();

  void append(const std::string &str);
  void append(const char *data, size_t len);
  void append(const void *data, size_t len);
  void ensureWritableBytes(size_t len);

  char* beginWrite();
  const char* beginWrite() const;
  void hasWriten(size_t len);
  void prepend(const void *data, size_t len);

  const char* findCRLF() const;


  void shrink(size_t reserve);


  ssize_t readFd(int fd, int* saveError);

private:
  char* begin();
  const char* begin() const;
  void makeSpace(size_t len);

  std::vector<char> buffer_;
  ssize_t readIndex_;
  ssize_t writeIndex_;

  static const char kCRLF[];

};

#endif /* BUFFER_H_ */
