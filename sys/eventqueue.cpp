#include "eventqueue.h"

#include "enumstr.h"
#include "logger.h"

#include <stdlib.h>
#include <string.h>

namespace sys
{

eventqueue *eventqueue::m_instance = static_cast<eventqueue*>(0);

bool eventqueue::createEventQueue()
{
	bool ret = false;

	if (m_instance == static_cast<eventqueue*>(0)) {
		m_instance = new eventqueue();

		ret = true;
	}

	return ret;
}

void eventqueue::destroyEventQueue()
{
	if (m_instance != static_cast<eventqueue*>(0)) {
		delete m_instance;
	}
}

eventqueue::eventqueue(void)
{
	int i;
	mutex_init(&m_mutex);

	for (i = 0; i < MAX_LISTENERS; i++) {
		m_listener[i].l = NULL;
		m_listener[i].id.setObjectId(TOKEN_LISTENER, INVALID_LISTENER_ID);
		m_listener[i].type = EVENT_NONE;
	}
}

eventqueue::~eventqueue(void)
{
	mutex_destroy(&m_mutex);
}

void eventqueue::push(const event& e)
{
//	printf("pushed event %d\n", e.getType());
	
	// dont push null events
	if (e.getType() != EVENT_NONE) {
		m_instance->lock();
		m_instance->m_queue.push(e);

		// move outside lock?
		m_instance->notifyListeners(e);
		m_instance->unlock();
	}
}

event eventqueue::pop()
{
	event e;

	m_instance->lock();

	if (!m_instance->m_queue.empty()) {
		e = m_instance->m_queue.front();
		m_instance->m_queue.pop();

//		printf("popped event %d\n", e.getType());
	}

	m_instance->unlock();

	return e;
}

void eventqueue::lock()
{
	mutex_lock(&m_mutex);
}

void eventqueue::unlock()
{
	mutex_unlock(&m_mutex);
}

bool eventqueue::empty()
{
	return m_queue.empty();
}

int eventqueue::getFreeTicket()
{
	int i;

	for (i = 0; i < MAX_LISTENERS; i++) {
		if ((!isValidObject(m_listener[i].id)) &&
			(m_listener[i].l == NULL)) {
			break;
		}
	}

	return i;
}

token eventqueue::registerListener(const event_type& type, listener *l)
{
	token id;
	int idx;

	lock();

	idx = getFreeTicket();

	if (idx != MAX_LISTENERS) {
		m_listener[idx].id.setIndex(idx);
		m_listener[idx].l = l;
		m_listener[idx].type = type;

		id = m_listener[idx].id;

		logger::log("registered listener %p for type %s at 0x%08x",
			l, enumstr::stringify(type).c_str(), m_listener[idx].id.getObjectId());
	}

	unlock();

	return id;
}

void eventqueue::unregisterListener(const token& id)
{
	unsigned int idx;

	lock();

	if (isValidObject(id)) {
		idx = id.getIndex();

		if (idx < MAX_LISTENERS) {
			m_listener[idx].id.setIndex(INVALID_LISTENER_ID);
			m_listener[idx].l = static_cast<listener *>(0);
			m_listener[idx].type = EVENT_NONE;
		}
	}

	unlock();
}

void eventqueue::notifyListeners(const event& e)
{
	int i;

	for (i = 0; i < MAX_LISTENERS; i++) {
		if ((isValidObject(m_listener[i].id)) &&
			(m_listener[i].l != NULL) &&
			(m_listener[i].type == e.getType())) {

//			logger::log("notifying listener %p for type %s",
//				m_listener[i].l, enumstr::stringify(e.getType()).c_str());

			m_listener[i].l->notify(e);
		}
	}
}

bool eventqueue::isValidObject(const token& obj)
{
	return ((obj.getIndex() != INVALID_LISTENER_ID) &&
			(obj.getType() == TOKEN_LISTENER));
}

}