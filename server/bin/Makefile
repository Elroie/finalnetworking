all : main

clean : 
	rm -rf main main.o

UNAME_S = (shell uname -s)

ifeq (UNAME_S, Darwin)
	CC = g++
endif 

ifeq (UNAME_S, Linux)
	CC = g++ -pthread
endif 

main : main.o 
	g++ -L../lib main.o -o main -llab8

%.o : %.cpp
	$(CC) -g -c -I../include $< -o $@