CXXFLAGS = -g -std=c++11 -Wall -fmessage-length=0 -pthread

OBJS =		main.o UniqueLock.o Observable.o StockFactory.o

LIBS =

TARGET =	MultiThreadSafe

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS) -pthread

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
