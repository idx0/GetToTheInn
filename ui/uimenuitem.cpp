#include "uimenuitem.h"

#include <libtcod.hpp>

namespace ui {

menuitem::menuitem(
		const char *label,
		unsigned int len,
		char key,
		widget *parent) :
	widget(parent),
	m_key(key),
	m_label_len(len)
{
	memset(m_label, 0, MAX_MENUITEM_LENGTH);
	memcpy(m_label, label, (len < MAX_MENUITEM_LENGTH ? len : MAX_MENUITEM_LENGTH));

	int w, h;
	TCODSystem::getCharSize(&w, &h);

	m_policy.setVerticalRule(policy::POLICY_FIXED);
	m_policy.setVerticalSize(h);
}

menuitem::~menuitem(void)
{
}

void menuitem::draw(canvas *console)
{
#ifdef PSEUDOCODE
	gltui *g = gltui::getInstance();
	uirect r = getSize();
	char buf[MAX_MENUITEM_LENGTH + 2];
	int offset = 2;

	if (isVisible()) {
		memset(buf, 0, MAX_MENUITEM_LENGTH + 2);

		g->prepareColor(COLOR_YELLOW, COLOR_BLACK);
		g->drawChar(' ', r.l, r.t);
		if (m_key == ' ') {
			g->drawText("space", 5, r.l + 1, r.t);
			offset += 4;
		} else {
			g->drawChar(m_key, r.l + 1, r.t);
		}

		_snprintf(buf, MAX_MENUITEM_LENGTH + 2, ": %s", m_label);
		g->prepareColor(COLOR_LIGHT_GRAY, COLOR_BLACK);
		g->drawText(buf, strlen(buf), r.l + offset, r.t);
	}
#endif
}

}
