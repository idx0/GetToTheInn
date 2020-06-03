#pragma once

#include "uiwidget.h"

namespace ui {

class panel : public widget
{
public:
	explicit panel(const gtti::Color& color = gtti::Color::black, widget *parent = NULL);
	virtual ~panel(void);

	virtual void setSize(const Rect &size);
	virtual void addChild(widget* child);

	void draw(canvas* console);

protected:
    gtti::Color m_color;
	canvas m_panel;
};

}
