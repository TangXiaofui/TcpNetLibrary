all:write read

clean:
	rm -f write read msg.pb.* .o log

proto_msg:
	protoc -I=. --cpp_out=. msg.proto

write:msg.pb.cc write.cpp
	g++ msg.pb.cc write.cpp -o write `pkg-config --cflags --libs protobuf`

read:msg.pb.cc read.cpp
	g++ msg.pb.cc read.cpp -o read `pkg-config --cflags --libs protobuf`
