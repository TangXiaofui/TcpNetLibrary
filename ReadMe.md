测试
	编译选项 -O2 -finline-limit = 1000  
	
	测试环境 vmware ubuntu14.04

	Intel(R) Core(TM) i7-6700HQ CPU @ 2.60GHz 单核， 内存3G
	
吞吐量
server single thread

pingpong 
	./pingpong_client 127.0.0.1 10000     1           4096        1000      60
						ip 		 port    threadnum   buf size     links    second     
libevent
  20455510016 total bytes read - client.cc:142
  4994021 total messages read - client.cc:143
  4096 average message size - client.cc:144
  325.131575521 MiB/s throughput - client.cc:146

tcpNetLibrary
  21956268032 total bytes read - client.cc:142
  5360452 total messages read - client.cc:143
  4095.97325599 average message size - client.cc:144
  348.985481771 MiB/s throughput - client.cc:146
	

事件处理
server  single thread 

webbench
cmd  ./webbench -1 -c 100 --get -t 60 http://127.0.0.1:8000/


Benchmarking: GET http://127.0.0.1:8000/
1000 clients, running 60 sec.

muduo
	Speed=591320 pages/min, 1871845 bytes/sec.
	Requests: 591107 susceed, 213 failed.

tcpNetLibrary
	Speed=434154 pages/min, 1359236 bytes/sec.
	Requests: 433798 susceed, 356 failed.

libevent
	Speed=918678 pages/min, 18404202 bytes/sec.
	Requests: 918678 susceed, 0 failed.


	并发数受到最大文件描述符的限制，socket BUF，数据量大小，读写文件速度
performent
valgrind --tool=callgrind --separate-threads=yes ./Multi...
python gprof2dot.py -f callgrind -n10 -s callgrind.out.31113 > valgrind.dot
dot -Tpng valgrind.dot -o valgrind.png

