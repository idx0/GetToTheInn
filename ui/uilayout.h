#pragma once

#include "uiwidget.h"

#include <vector>

namespace ui {

enum layout_type
{
	LAYOUT_VERTICAL = 0,
	LAYOUT_HORIZONTAL
};

class layout : public widget
{
public:
	explicit layout(const Rect &size, layout_type type, widget *parent = NULL);
	virtual ~layout(void);

	virtual void addChild(widget *child);

	virtual void draw(canvas* console);
	virtual void drawChildren(canvas* console);

	void setSize(const Rect &size);

	void redrawNotify();
protected:
	void updateLayouts();

private:
	std::vector<Rect> m_layout;
	std::vector<widget *> m_children;

	layout_type m_layout_type;
};

}
