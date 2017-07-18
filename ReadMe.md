测试
	编译选项 -g -finline-limit = 1000  
	
	测试环境 vmware ubuntu14.04

	Intel(R) Core(TM) i7-6700HQ CPU @ 2.60GHz 单核， 内存3G
	
吞吐量
pingpong 

libevent
	

事件处理
webbench
cmd  ./webbench -1 -c 100 --get -t 60 http://127.0.0.1:8000/


	并发数受到最大文件描述符的限制，socket BUF，数据量大小，读写文件速度
