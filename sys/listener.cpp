#include "listener.h"
#include "eventqueue.h"
#include "logger.h"
#include "enumstr.h"

namespace sys
{

listener::listener(void *tag)
{
	int i;
	m_tag = tag;

	for (i = 0; i < EVENT_COUNT; i++) {
		m_callback[i].callback.null_listener = NULL;
		m_callback[i].callback.obj_listener = NULL;
		m_callback[i].callback.data_listener = NULL;
		m_callback[i].id.setObjectId(TOKEN_LISTENER, INVALID_LISTENER_ID);
	}

	mutex_init(&m_mutex);
}

listener::~listener(void)
{
	mutex_destroy(&m_mutex);
}

void listener::nullListeners(const event_type& type)
{
	m_callback[type].callback.null_listener = NULL;
	m_callback[type].callback.obj_listener = NULL;
	m_callback[type].callback.data_listener = NULL;
}

void listener::addListener(
	const event_type& type,
	ptr_listener_func null_listener)
{
	nullListeners(type);
	m_callback[type].callback.null_listener = null_listener;

	if (!isValidObject(m_callback[type].id)) {
		registerListener(type);
	}
}

void listener::addListener(
	const event_type& type,
	object_listener_func obj_listener)
{
	nullListeners(type);
	m_callback[type].callback.obj_listener = obj_listener;

	if (!isValidObject(m_callback[type].id)) {
		registerListener(type);
	}
}

void listener::addListener(
	const event_type& type,
	data_listener_func data_listener)
{
	nullListeners(type);
	m_callback[type].callback.data_listener = data_listener;

	if (!isValidObject(m_callback[type].id)) {
		registerListener(type);
	}
}

void listener::removeListener(const event_type& type)
{
	eventqueue *q = eventqueue::getInstance();

	if (q != static_cast<eventqueue*>(0)) {
		q->unregisterListener(m_callback[type].id);
;
		m_callback[type].callback.obj_listener = NULL;
		m_callback[type].callback.null_listener = NULL;
		m_callback[type].id.setIndex(INVALID_LISTENER_ID);
	}
	
}

bool listener::registerListener(const event_type& type)
{
	bool ret = false;
	eventqueue *q = eventqueue::getInstance();

	if (q != static_cast<eventqueue*>(0)) {
		m_callback[type].id = q->registerListener(type, this);

		ret = true;
	}

	return ret;
}

void listener::notify(const event& e)
{
	mutex_lock(&m_mutex);
//	logger::log("listener: event %s pushed", enumstr::stringify(e.getType()).c_str());
	m_listenQueue.push(e);
	mutex_unlock(&m_mutex);
}

void listener::pop()
{
	mutex_lock(&m_mutex);
	if (!m_listenQueue.empty()) {
		event e = m_listenQueue.front();
		m_listenQueue.pop();

		bool success = false;

		if (isValidObject(m_callback[e.getType()].id)) {
			switch (e.getType()) {
			default:
				success = call(m_callback[e.getType()].callback.null_listener, e.getPayload());
				break;
			case EVENT_MOUSE_LBUTTON_DOWN:
			case EVENT_MOUSE_LBUTTON_UP:
			case EVENT_MOUSE_MBUTTON_DOWN:
			case EVENT_MOUSE_MBUTTON_UP:
			case EVENT_MOUSE_RBUTTON_DOWN:
			case EVENT_MOUSE_RBUTTON_UP:
			case EVENT_RESIZE:
			case EVENT_MOUSE_MOVE:
			case EVENT_KEY_DOWN:
			case EVENT_KEY_UP:
			case EVENT_UPDATE:
			case EVENT_ACTION:
			case EVENT_LIGHTING:
				success = call(m_callback[e.getType()].callback.data_listener, e.getPayload());
				break;
			case EVENT_DATA_CREATE:
			case EVENT_DATA_DESTROY:
			case EVENT_DATA_READ:
			case EVENT_DATA_WRITE:
			case EVENT_DATA_SAFE:
			case EVENT_DATA_UNSAFE:
			case EVENT_RENDER:
				success = call(m_callback[e.getType()].callback.obj_listener, e.getPayload());
				break;
			}
		}

		if (success) {
//			logger::log("listener: event %s popped", enumstr::stringify(e.getType()).c_str());
		}
	}
	mutex_unlock(&m_mutex);
}

bool listener::call(ptr_listener_func f, const event_payload& p)
{
	bool ret = false;

	if (f) { 
		f(m_tag, p.ptr);
		ret = true;
	}

	return ret;
}

bool listener::call(object_listener_func f, const event_payload& p)
{
	bool ret = false;

	if (f) { 
		f(m_tag, p.token);
		ret = true;
	}

	return ret;
}

bool listener::call(data_listener_func f, const event_payload& p)
{
	bool ret = false;

	if (f) { 
		f(m_tag, p.data);
		ret = true;
	}

	return ret;
}

bool listener::isValidObject(const token& obj)
{
	return ((obj.getIndex() != INVALID_LISTENER_ID) &&
			(obj.getType() == TOKEN_LISTENER));
}

}
