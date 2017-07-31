all : liblab8.a cplib


clean : 
	rm -rf .o .a

UNAME_S = (shell uname -s)

ifeq (UNAME_S, Darwin)
	CC = g++
endif 

ifeq (UNAME_S, Linux)
	CC = g++ -pthread
endif 

cplib : liblab8.a
	cp liblab8.a ../lib

liblab8.a : UserRepository.o 
	ar crv $@ UserRepository.o

%.o : %.cpp
	$(CC) -g -c -I../include $< -o $@