#include "uilabel.h"

namespace ui {

	label::label(const std::string& sz, widget *parent) :
		widget(parent),
		m_text(sz),
		m_color(Color::white)
	{
	}

	label::~label()
	{
	}

	void label::draw(canvas *console)
	{
		if (isVisible()) {
			console->drawText(m_size, m_text, m_color);

			drawChildren(console);
		}
	}

}
