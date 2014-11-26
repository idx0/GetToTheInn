#pragma once

#include "thread_os.h"
#include "event.h"
#include "token.h"

#include <queue>

namespace sys
{

typedef void (*ptr_listener_func)(void *tag, const void *ptr);
typedef void (*object_listener_func)(void *tag, const token_id);
typedef void (*data_listener_func)(void *tag, const event_payload::_data&);

typedef unsigned long listener_id;

#define INVALID_LISTENER_ID TOKEN_INDEX_MASK

// This class provides a mechanism for asynchronous callbacks triggered
// off event messages being pushed on the queue.  It is designed to provide
// behind-the-scenes management of queue events for threads.
//
// Threads can add a listener of any type for each event (although certain
// events will not call into certain callbacks).  When an event is pushed,
// all listeners listening on that event type get notified, and the event
// gets placed in the local listen queue.
//
// CALLBACKS
// Callback functions are c-style.  The void *m_tag variable can be set
// to the pointer to the callback class to help call member functions.
// Ideally, all callback functions should be non-blocking as the event
// queue will be locked for the entirety of the call.  Callbacks must NOT
// contain rendering code.
class listener
{
public:
	listener(void *tag);
	virtual ~listener(void);

	void addListener(
		const event_type& type,
		ptr_listener_func null_listener);
	void addListener(
		const event_type& type,
		object_listener_func obj_listener);
	void addListener(
		const event_type& type,
		data_listener_func data_listener);

	void removeListener(const event_type& type);

	token getId(const event_type& type) { return m_callback[type].id; }

	// pops and event off the listen queue
	void pop();
public:
	virtual void notify(const event& e);

protected:
	struct listener_callback
	{
		ptr_listener_func null_listener;
		object_listener_func obj_listener;
		data_listener_func data_listener;
	};

	struct listener_set
	{
		listener_callback callback;
		token id;
	};

	listener_set m_callback[EVENT_COUNT];
	std::queue<event> m_listenQueue;
private:
	bool registerListener(const event_type& type);

	bool call(ptr_listener_func f, const event_payload& p);
	bool call(object_listener_func f, const event_payload& p);
	bool call(data_listener_func f, const event_payload& p);

	bool isValidObject(const token& obj);
	void nullListeners(const event_type& type);

	void *m_tag;

	mutex m_mutex;
};

}
