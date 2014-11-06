#pragma once

#include <list>

#include "uiwidget.h"
#include "../sys/thread.h"

namespace ui
{

class uithread : public sys::thread
{
public:
	uithread();
	~uithread();

	// called by the render thread to render the ui
	void render(canvas* console);

	// take ownership of a widget (the widget will be automatically
	// deleted when the thread dies)
	void own(widget* child);

	// pop events and handle them by triggering widget
	// callbacks
	void thread_func();

protected:

	std::list<widget*> m_widgets;
};

}
