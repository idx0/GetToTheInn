#include "enumstr.h"

using namespace enumstr;

const std::string enumstr::stringify(const sys::event_type& t)
{
	static const std::string names[] = {
		"EVENT_NONE",
		"EVENT_MOUSE_LBUTTON_DOWN",
		"EVENT_MOUSE_LBUTTON_UP",
		"EVENT_MOUSE_MBUTTON_DOWN",
		"EVENT_MOUSE_MBUTTON_UP",
		"EVENT_MOUSE_RBUTTON_DOWN",
		"EVENT_MOUSE_RBUTTON_UP",
		"EVENT_MOUSE_MOVE",
		"EVENT_KEY_DOWN",
		"EVENT_KEY_UP",
		"EVENT_RESIZE",
		"EVENT_RENDER",
		"EVENT_UPDATE",
		"EVENT_LIGHTING",
		"EVENT_ACTION",
		"EVENT_WORK",
		"EVENT_DATA_CREATE",
		"EVENT_DATA_DESTROY",
		"EVENT_DATA_READ",
		"EVENT_DATA_WRITE",
		"EVENT_DATA_SAFE",
		"EVENT_DATA_UNSAFE"
	};
	
	static const unsigned int count = sizeof(names) / sizeof(char *);
	
	return (static_cast<unsigned int>(t) >= count ?
		"UNKNOWN" : names[t]);
}
