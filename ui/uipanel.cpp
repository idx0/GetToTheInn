#include "uipanel.h"

namespace ui {

panel::panel(const Color& color, widget *parent)
	: widget(parent),
	  m_color(color),
	  m_panel(NULL)
{
}

panel::~panel(void)
{
	delete m_panel.console();
}

void panel::addChild(widget *child)
{
	if (child != NULL) {
		m_child = child;
		m_child->setSize(Rect(0, 0, m_size.height(), m_size.width()));
	}
}

void panel::setSize(const Rect &size)
{
	widget::setSize(size);
	
	TCODConsole* nc = new TCODConsole(size.width(), size.height());
	nc->setDefaultBackground(m_color.toTCOD());
	nc->clear();

	delete m_panel.console();
	m_panel = canvas(nc);
}

void panel::draw(canvas* console)
{
	if (isVisible()) {
		m_panel.console()->clear();

		// draw children to the panel console
		drawChildren(&m_panel);

		// blit our panel to the main console
		TCODConsole::blit(m_panel.console(), 0, 0, 0, 0, console->console(), m_size.left(), m_size.top());
	}
}

}
