#include "MThread.h"

using namespace std;
/*
 * Method to run on a different thread.
 */
void* worker(void* arg) {
	MThread* threadObj = (MThread*) arg;
	threadObj->run();
	threadObj->threadId = 0;
	return NULL;
}

/*
 * Start the thread
 */
void MThread::start() {
	pthread_create(&threadId, NULL, worker, (void*) this);
}

/**
 * Block current thread until this thread is finished
 */
void MThread::waitForThread() {
	pthread_join(threadId, NULL);
	threadId = 0;
}

MThread::~MThread() {
	if (threadId > 0) {
		// Kills the thread if exist
		pthread_cancel(threadId);
	}
}
