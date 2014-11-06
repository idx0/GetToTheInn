#include "uiframe.h"

namespace ui {

frame::frame(const char *name, unsigned int len,
			 frame_type type, widget *parent)
		: widget(parent),
		  m_frame_type(type),
		  m_length(len)
{
	memset(m_title, 0, 255);
	memcpy_s(m_title, 255, name, len);
}

frame::~frame(void)
{
}

void frame::drawChildren(canvas *console)
{
	// remove a tile on each side
	Rect r = getSize().shrink(1);

	if (m_child != NULL) {
		if (r != m_framerect) {
			m_framerect = r;
			m_child->setSize(r);
		}

		m_child->draw(console);
	}
}

int frame::getFramePiece(frame_position p)
{
	int c;

	switch (p) {
	case FRAME_TOPLEFT:
		if (m_frame_type == FRAME_TYPE_THIN) { c = 218; }
		else { c = 201; }
		break;
	case FRAME_TOP:
	case FRAME_BOTTOM:
		if (m_frame_type == FRAME_TYPE_THIN) { c = 196; }
		else { c = 205; }
		break;
	case FRAME_TOPRIGHT:
		if (m_frame_type == FRAME_TYPE_THIN) { c = 191; }
		else { c = 187; }
		break;
	case FRAME_LEFT:
	case FRAME_RIGHT:
		if (m_frame_type == FRAME_TYPE_THIN) { c = 179; }
		else { c = 186; }
		break;
	case FRAME_BOTTOMLEFT:
		if (m_frame_type == FRAME_TYPE_THIN) { c = 192; }
		else { c = 200; }
		break;
	case FRAME_BOTTOMRIGHT:
		if (m_frame_type == FRAME_TYPE_THIN) { c = 217; }
		else { c = 188; }
		break;
	}

	return c;
}

void frame::draw(canvas *console)
{
#if PSEUDOCODE
	int width = m_size.r - m_size.l;
	int height = m_size.b - m_size.t;
	int maxtext = width - 6;
	int i, len = (static_cast<int>(m_length) < maxtext ?
		static_cast<int>(m_length) :
		maxtext);
	gltui *g = gltui::getInstance();

	if (width < 7) {
		// we can't draw anything, just draw the child
	} else {
		g->prepareColor(COLOR_LIGHT_GRAY, COLOR_BLACK);

		/* top row */
		g->drawChar(getFramePiece(FRAME_TOPLEFT), m_size.l, m_size.t);
		g->drawChar(getFramePiece(FRAME_TOP), m_size.l + 1, m_size.t);
		g->drawChar(' ', m_size.l + 2, m_size.t);
		g->drawText(m_title, len, m_size.l + 3, m_size.t);
		g->drawChar(' ', m_size.l + 2, m_size.t);
		for (i = 0; i < (maxtext - len); i++) {
			g->drawChar(getFramePiece(FRAME_TOP), m_size.l + len + 4 + i, m_size.t);
		}
		g->drawChar(getFramePiece(FRAME_TOP), m_size.r - 2, m_size.t);
		g->drawChar(getFramePiece(FRAME_TOPRIGHT), m_size.r - 1, m_size.t);

		/* middle rows */
		for (i = 0; i < (height - 2); i++) {
			g->drawChar(getFramePiece(FRAME_LEFT), m_size.l, m_size.t + i + 1);
			g->drawChar(getFramePiece(FRAME_RIGHT), m_size.r - 1, m_size.t + i + 1);
		}

		/* bottom row */
		g->drawChar(getFramePiece(FRAME_BOTTOMLEFT), m_size.l, m_size.b - 1);
		for (i = 0; i < (width - 2); i++) {
			g->drawChar(getFramePiece(FRAME_BOTTOM), m_size.l + i + 1, m_size.b - 1);
		}
		g->drawChar(getFramePiece(FRAME_BOTTOMRIGHT), m_size.r - 1, m_size.b - 1);
	}

	
#endif
	int maxlen = m_size.width() - 6;
	int i = 0, len = (int)std::min(m_length, (unsigned int)maxlen);

	if (m_size.width() < 7) {
		// we can't draw anything, just draw the child
	} else {

	}

	if (isVisible()) {
		drawChildren(console);
	}
}

}