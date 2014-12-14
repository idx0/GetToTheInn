#pragma once

#include "platform.h"
#include "event.h"
#include "listener.h"

#include <queue>

#define MAX_LISTENERS	1024

namespace sys
{

class eventqueue
{
public:
	static bool createEventQueue();
	static void destroyEventQueue();

	static void push(const event& e);
	static event pop();

	bool empty();

	token registerListener(const event_type& type, listener *l);
	void unregisterListener(const token& id);

	static eventqueue *getInstance() { return m_instance; }
protected:
	eventqueue(void);
	virtual ~eventqueue(void);

	void lock();
	void unlock();

	static eventqueue *m_instance;

	void notifyListeners(const event& e);
private:
	std::queue<event> m_queue;
	
	struct listener_ticket
	{
		listener *l;
		token id;
		event_type type;
	};

	listener_ticket m_listener[MAX_LISTENERS];

	int getFreeTicket();
	bool isValidObject(const token& obj);

	mutex m_mutex;
};

}