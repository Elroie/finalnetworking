all : liblab8.a cplib


clean : 
	rm -rf .o .a

UNAME_S = (shell uname -s)

ifeq (UNAME_S, Darwin)
	CC = g++ -std=c++11
endif 

ifeq (UNAME_S, Linux)
	CC = g++ -std=c++11 -pthread
endif 

cplib : liblab8.a
	cp liblab8.a ../lib

liblab8.a : UserRepository.o ../../Shared/src/MThread.o
	ar crv $@ UserRepository.o ../../Shared/src/MThread.o

%.o : %.cpp
	$(CC) -g -c -I../include -I../../Shared/include $< -o $@