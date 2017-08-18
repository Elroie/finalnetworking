#include <strings.h>
#include <map>
#include "UDPSocket.h"
#include "MThread.h"
#include "Broker.h"
#include <set>
#include "Dispatcher.h"

class Broker;

using namespace std;

class Dispatcher : public MThread{
	set<Broker*> brokers;

public:

	virtual ~TCPMessengerDispatcher();

	/*
	 * Create a broker.
	 */
	void createBroker(UDPSocket* firstSocket, UDPSocket* secondSocket);

	/**
	 * Close broker.
	 */
	void closeBroker(Broker* broker);

	/**
	 * Shutdown.
	 */
	void shutdown();

	/**
	 * Delete broker.
	 */
	void deleteBroker(Broker* broker);
};
