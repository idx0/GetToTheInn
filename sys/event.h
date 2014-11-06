#pragma once
#include "token.h"

namespace sys
{

enum event_type
{
	EVENT_FIRST = 0,

	EVENT_NONE = EVENT_FIRST,

	EVENT_MOUSE_LBUTTON_DOWN,
	EVENT_MOUSE_LBUTTON_UP,
	EVENT_MOUSE_MBUTTON_DOWN,
	EVENT_MOUSE_MBUTTON_UP,
	EVENT_MOUSE_RBUTTON_DOWN,
	EVENT_MOUSE_RBUTTON_UP,
	EVENT_MOUSE_MOVE,
	EVENT_KEY_DOWN,
	EVENT_KEY_UP,
	EVENT_RESIZE,

	EVENT_RENDER,
	EVENT_UPDATE,
	EVENT_ACTION,	// player action
	EVENT_WORK,

	EVENT_DATA_CREATE,
	EVENT_DATA_DESTROY,
	EVENT_DATA_READ,
	EVENT_DATA_WRITE,
	EVENT_DATA_SAFE,
	EVENT_DATA_UNSAFE,

	EVENT_LAST,
	EVENT_COUNT = (EVENT_LAST - EVENT_FIRST),

	EVENT_CUSTOM = (EVENT_LAST + 1)
};

typedef union {
	void *ptr;
	token_id token;

	struct _data {
		unsigned short param1;
		unsigned short param2;
		unsigned int flags;
	} data;
} event_payload;

class event
{
public:
	explicit event(
		const event_type& type,
		const event_payload& payload);
	explicit event();
	virtual ~event();
	
	event_payload getPayload() const { return m_payload; }

	void setPayload(const event_payload& payload);
	void setType(const event_type& type);

	event_type getType() const { return m_type; }
	token getObjectId() { return m_object; }

	event &operator=(const event &copyFrom);
protected:
	event_type m_type;
	event_payload m_payload;
	token m_object;
};

}