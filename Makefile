CXXFLAGS = -g -std=c++11 -Wall -fmessage-length=0 -pthread -lpthread

OBJS =		main.o UniqueLock.o Observable.o StockFactory.o singleton.o pthreadKey.o

LIBS =

TARGET =	MultiThreadSafe

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS) -pthread -lpthread

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
