CXX=clang++
CFLAGS=-O3 -std=c++0x -pg -D_DEBUG -g -c -Wall

all : 
	make -C ./client all
	
clean : 
	make -C ./client clean

