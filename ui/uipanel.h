#pragma once

#include <libtcod/libtcod.hpp>

#include "uiwidget.h"

namespace ui {

class panel : public widget
{
public:
	explicit panel(const Color& color = Color::black, widget *parent = NULL);
	virtual ~panel(void);

	virtual void setSize(const Rect &size);
	virtual void addChild(widget* child);

	void draw(canvas* console);

protected:
	Color m_color;
	canvas m_panel;
};

}
