#include "uipanel.h"
#include "../TileEngine.h"

namespace ui {

panel::panel(const gtti::Color& color, widget *parent)
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
	
    Console *nc = TileEngine::createConsole(size.width(), size.height());
    nc->setBackgroundColor(gtti::Color::black);

	delete m_panel.console();
	m_panel = canvas(nc);
}

void panel::draw(canvas* console)
{
	if (isVisible()) {
        m_panel.console()->clear();

		// draw children to the panel console
		drawChildren(&m_panel);

        console->console()->apply(m_panel.console(), m_size.left(), m_size.top());
	}
}

}
