#!/usr/bin/env python3
# -*- coding: utf-8 -*-


import threading
import socket
import os 
from sys import argv

print("input <port> <bufsize>")
filename,port,num = argv


HOST,PORT = 'localhost',10000

def connectToServer(i):
    try:
        sockfd = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        sockfd.connect((HOST,PORT))
        s = 'a' * int(num)
        sockfd.sendall(bytes(s,encoding="utf-8"))
        data = sockfd.recv(int(num)+1)
        sockfd.close()
    except socket.error as arg:
        print ("Connect server failed: %s "%(arg))
        os._exit(-1)
    finally:
        sockfd.close()

threads = []
for i in range(1,50000):
    t = threading.Thread(target=connectToServer,args=(i,))
    threads.append(t)

if __name__ == '__main__':
    print("pid = %s"%(os.getpid()))
    import datetime
    starttime = datetime.datetime.now()
    for t in threads:
        t.start()
    for t in threads:
        t.join()
    endtime = datetime.datetime.now()
    print ((endtime - starttime).seconds)


