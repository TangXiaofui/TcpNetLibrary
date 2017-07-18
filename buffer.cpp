/*
 * buffer.cpp
 *
 *  Created on: Jul 8, 2017
 *      Author: txh
 */


#include "buffer.h"
#include <algorithm>
#include <assert.h>
#include <sys/uio.h>

const char Buffer::kCRLF[] = "\r\n";

Buffer::Buffer()
:buffer_(kCheapPrepared + kInitialSize),
 readIndex_(kCheapPrepared),
 writeIndex_(kCheapPrepared)
{
  assert(readableBytes() == 0);
  assert(writableBytes() == kInitialSize);
  assert(prependableBytes() == kCheapPrepared);
}

void  Buffer::swap(Buffer& rhs)
{
  buffer_.swap(rhs.buffer_);
  std::swap(readIndex_, rhs.readIndex_);
  std::swap(writeIndex_, rhs.writeIndex_);
}

size_t  Buffer::readableBytes() const
{
  return writeIndex_ - readIndex_;
}

size_t  Buffer::writableBytes() const
{
  return buffer_.size() - writeIndex_ ;
}

size_t Buffer::prependableBytes() const
{
  return readIndex_;
}

const char* Buffer::peek() const
{
  return begin() + readIndex_;
}

void  Buffer::retrieve(size_t len)
{
  assert(len <= readableBytes());
  readIndex_ += len;
}


void Buffer::retrieveUntil(const char* end)
{
  assert(peek() <= end);
  assert(end <= beginWrite());
  retrieve(end - peek());
}

void  Buffer::retrieveAll()
{
  readIndex_ = kCheapPrepared;
  writeIndex_ = kCheapPrepared;
}

std::string Buffer::retrieveAsString()
{
  std::string str(peek(),readableBytes());
  retrieveAll();
  return str;
}

void  Buffer::append(const std::string &str)
{
  append(str.c_str(),str.length());
}

void Buffer::append(const char *data, size_t len)
{
  ensureWritableBytes(len);
  std::copy(data,data + len,beginWrite());
  hasWriten(len);
}

void  Buffer::append(const void *data,size_t len)
{
  append(static_cast<const char*>(data),len);
}

void Buffer::ensureWritableBytes(size_t len)
{
  if(writableBytes() < len)
    makeSpace(len);
  assert(writableBytes() >= len);
}

char*  Buffer::beginWrite()
{
  return begin() + writeIndex_;
}

const char*  Buffer::beginWrite() const
{
  return begin() + writeIndex_;
}

void  Buffer::hasWriten(size_t len)
{
  writeIndex_ += len;
}

void  Buffer::prepend(const void *data, size_t len)
{
  assert(len <= prependableBytes());
  readIndex_ -= len;
  const char *d = static_cast<const char*>(data);
  std::copy(d,d+len,begin()+readIndex_);
}


void  Buffer::shrink(size_t reserve)
{
  std::vector<char> buf(kCheapPrepared + readableBytes() + reserve);
  std::copy(peek(),peek()+readableBytes(),buf.begin()+kCheapPrepared);
  buf.swap(buffer_);
}


ssize_t  Buffer::readFd(int fd, int* saveError)
{
  char extraBuf[65536];
  struct iovec vec[2];
  const size_t writable = writableBytes();
  vec[0].iov_base = begin() + writeIndex_;
  vec[0].iov_len = writable;
  vec[1].iov_base = extraBuf;
  vec[1].iov_len = sizeof extraBuf;

  const ssize_t n = readv(fd,vec,2);
  if(n < 0)
    {
      *saveError = errno;
    }
  else if(implicit_cast<size_t>(n) <= writable){
      writeIndex_ += n;
  }
  else
    {
      writeIndex_ = buffer_.size();
      append(extraBuf,n-writable);
    }
  return n;

}


char*  Buffer::begin()
{
  return &*buffer_.begin();
}

const char*  Buffer::begin() const
{
  return &*buffer_.begin();
}

void  Buffer::makeSpace(size_t len)
{
  if(writableBytes() + prependableBytes() < len + kCheapPrepared)
    {
      buffer_.resize(writeIndex_ + len);
    }
  else{
      assert(kCheapPrepared < readIndex_);
      size_t readable = readableBytes();
      std::copy(begin()+readIndex_,begin()+writeIndex_,begin()+kCheapPrepared);
      readIndex_ = kCheapPrepared;
      writeIndex_ = readIndex_ + readable;
      assert(readable == readableBytes());
  }
}


const char* Buffer::findCRLF() const
{
  const char* crlf = std::search(peek(), beginWrite(), kCRLF, kCRLF+2);
  return crlf == beginWrite() ? NULL : crlf;

}

