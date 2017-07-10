#!/usr/bin/env python3
# -*- coding: utf-8 -*-


import threading
import socket
import os 
HOST,PORT = 'localhost',2000

def connectToServer(i):
    try:
        sockfd = socket.socket(socket.AF_INET,socket.SOCK_STREAM)
        sockfd.connect((HOST,PORT))
        sockfd.sendall(bytes("abc",encoding="utf-8"))
        data = sockfd.recv(1024)
        print ("%s %s"%(i,data))
        sockfd.close()
    except socket.error as arg:
        print ("Connect server failed: %s "%(arg))
        os._exit(-1)
    finally:
        sockfd.close()

threads = []
for i in range(1,1000000):
    t = threading.Thread(target=connectToServer,args=(i,))
    threads.append(t)

if __name__ == '__main__':
    for t in threads:
        t.start()
    for t in threads:
        t.join()
    print("done")

