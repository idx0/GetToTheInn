#include "uiframe.h"

namespace ui {

const int frame::m_sFRAME_CHAR[8] = {
	201,	// FRAME_TOPLEFT
	205,	// FRAME_TOP
	187,	// FRAME_TOPRIGHT
	186,	// FRAME_LEFT
	186,	// FRAME_RIGHT
	200,	// FRAME_BOTTOMLEFT
	205,	// FRAME_BOTTOM
	188		// FRAME_BOTTOMRIGHT
};

frame::frame(const char *name, unsigned int len,
			 frame_type type, widget *parent)
		: widget(parent),
		  m_frame_type(type),
		  m_length(len)
{
	memset(m_title, 0, 255);
	memcpy_s(m_title, 255, name, len);
}

frame::frame(frame_type type, widget *parent)
		: widget(parent),
		  m_frame_type(type),
		  m_length(0)
{
	memset(m_title, 0, 255);
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
		}

		m_child->setSize(m_framerect);
		m_child->draw(console);
	}
}

void frame::setColor(const Color& color)
{
	m_color = color;
}

void frame::draw(canvas *console)
{
	if (isVisible()) {
		int maxlen = m_size.width() - 6;
		int i = 1, len = (int)std::min(m_length, (unsigned int)maxlen);

		if (m_size.width() < 7) {
			// we can't draw anything, just draw the child
		} else {
			console->draw(m_size.topLeft(), m_sFRAME_CHAR[FRAME_TOPLEFT], m_color);

			if (m_length > 0) {
				Point tl = m_size.topLeft() + Point(3, 0);
				Point br = tl + Point(maxlen, 1);

				Rect r(tl, br); 
				console->draw(m_size.topLeft() + Point(1, 0), m_sFRAME_CHAR[FRAME_TOP], m_color);
				console->drawText(r, m_title, m_color);

				i = 4 + len;
			}

			for (; i < m_size.width() - 1; i++) {
				console->draw(m_size.topLeft() + Point(i, 0), m_sFRAME_CHAR[FRAME_TOP], m_color);
			}

			console->draw(Point(m_size.right() - 1, m_size.top()),
								m_sFRAME_CHAR[FRAME_TOPRIGHT], m_color);

			// middle rows
			for (i = 0; i < (m_size.height() - 2); i++) {

				console->draw(Point(m_size.left(), m_size.top() + i + 1),
									m_sFRAME_CHAR[FRAME_LEFT], m_color);
				console->draw(Point(m_size.right() - 1, m_size.top() + i + 1),
									m_sFRAME_CHAR[FRAME_RIGHT], m_color);
			}

			// bottom row
			console->draw(Point(m_size.left(), m_size.bottom() - 1),
								m_sFRAME_CHAR[FRAME_BOTTOMLEFT], m_color);
			for (i = 0; i < (m_size.width() - 2); i++) {
				console->draw(Point(m_size.left() + i + 1, m_size.bottom() - 1),
									m_sFRAME_CHAR[FRAME_BOTTOM], m_color);
			}
			console->draw(Point(m_size.right() - 1, m_size.bottom() - 1),
								m_sFRAME_CHAR[FRAME_BOTTOMRIGHT], m_color);
		}

		drawChildren(console);
	}
}

}