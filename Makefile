CXXFLAGS = -g -std=c++11 -Wall -fmessage-length=0 -pthread -lpthread

OBJS =		main.o UniqueLock.o Observable.o StockFactory.o singleton.o \
pthreadKey.o utility.o eventLoop.o logging.o channel.o poller.o timeStamp.o \
timerId.o timer.o timerQueue.o eventLoopThread.o netAddress.o socket.o acceptor.o \
tcpConnection.o tcpServer.o buffer.o eventLoopThreadPool.o connector.o

LIBS =

TARGET =	MultiThreadSafe

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS) -pthread -lpthread

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
