all : liblab10.a cplib


clean : 
	rm -rf *.o *.a

UNAME_S = (shell uname -s)

ifeq (UNAME_S, Darwin)
	CC = g++
endif 

ifeq (UNAME_S, Linux)
	CC = g++ -pthread
endif 

cplib : liblab10.a
	cp liblab10.a ../lib

liblab10.a : Broker.o TCPMessengerDispatcher.o TCPMessengerServer.o TCPSocket.o MThread.o Dispatcher.o ExtendedMultipleTCPSocketListener.o MultipleTCPSocketsListener.o UDPSocket.o
	ar crv $@ Broker.o TCPMessengerDispatcher.o TCPMessengerServer.o TCPSocket.o MThread.o Dispatcher.o ExtendedMultipleTCPSocketListener.o MultipleTCPSocketsListener.o UDPSocket.o

%.o : %.cpp
	$(CC) -g -c -I../include $< -o $@	
