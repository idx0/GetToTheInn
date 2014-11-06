#include "event.h"

#include <stdio.h>
#include <string.h>

namespace sys
{

static unsigned int event_count = 0;

event::event(
	const event_type& type,
	const event_payload& payload) :
	m_type(type),
	m_payload(payload)
{
	// nothing
	m_object.setObjectId(TOKEN_EVENT, event_count);

	event_count++;
	event_count &= TOKEN_INDEX_MASK;
}

event::event() : m_type(EVENT_NONE)
{
	event_payload p;
	p.ptr = NULL;

	m_payload = p;
	
	m_object.setObjectId(TOKEN_EVENT, event_count);

	event_count++;
	event_count &= TOKEN_INDEX_MASK;
}

event::~event()
{
	// nothing
}

event &event::operator=(const event &copyFrom)
{
	if (this != &copyFrom) {
		m_type = copyFrom.m_type;
//		memcpy(&m_payload, &copyFrom.m_payload, sizeof(event_payload));
		m_payload = copyFrom.m_payload;
		m_object = copyFrom.m_object;
	}

	return (*this);
}

void event::setPayload(const event_payload& payload)
{
	memcpy(&m_payload, &payload, sizeof(event_payload));
}

void event::setType(const event_type& type)
{
	m_type = type;
}

}