#include <pthread.h>
#include <iostream>
#include <stdio.h>

class MThread {
public:
	pthread_t threadId;

public:
	
	virtual void run() = 0;

	/*
	 * Start the thread
	 */
	void start();

	/**
	 * Block current thread until this thread is finished
	 */
	void waitForThread();
	virtual ~MThread();
};
