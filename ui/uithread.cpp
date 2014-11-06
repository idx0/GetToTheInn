#pragma once

#include "uithread.h"

namespace ui
{
	uithread::uithread() : sys::thread(THREAD_JOINABLE)
	{
	}

	uithread::~uithread()
	{
		std::list<widget*>::iterator it = m_widgets.begin();

		for (; it != m_widgets.end(); it++) {
			delete (*it);
		}

		m_widgets.clear();
	}

	void uithread::render(canvas* console)
	{
		if (sys::mutex_trylock(&m_mutex) == 0) {
			std::list<widget*>::iterator it = m_widgets.begin();

			for (; it != m_widgets.end(); it++) {
				(*it)->draw(console);
			}

			sys::mutex_unlock(&m_mutex);
		}
	}

	void uithread::own(widget* child)
	{
		m_widgets.push_back(child);
	}

	void uithread::thread_func()
	{
		// TODO
	}
}