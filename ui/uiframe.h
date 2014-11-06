#pragma once

#include "uiwidget.h"

namespace ui {

enum frame_type
{
	FRAME_TYPE_THIN = 0,
	FRAME_TYPE_THICK
};

enum frame_position
{
	FRAME_TOPLEFT = 0,
	FRAME_TOP,
	FRAME_TOPRIGHT,
	FRAME_LEFT,
	FRAME_RIGHT,
	FRAME_BOTTOMLEFT,
	FRAME_BOTTOM,
	FRAME_BOTTOMRIGHT
};

class frame : public widget
{
public:
	explicit frame(const char *name, unsigned int len,
		frame_type type, widget *parent = NULL);
	virtual ~frame(void);

	virtual void draw(canvas *console);
	virtual void drawChildren(canvas *console);

	void setTitle(const char *name, unsigned int len);
protected:
	int getFramePiece(frame_position p);

private:
	frame_type m_frame_type;
	char m_title[255];
	unsigned int m_length;

	Rect m_framerect;
};

}
