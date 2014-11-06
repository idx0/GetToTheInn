#pragma once

#include "uiwidget.h"

#define MAX_MENUITEM_LENGTH 256

namespace ui {

class menuitem : public widget
{
public:
	explicit menuitem(
		const char *label,
		unsigned int len,
		char key,
		widget *parent = NULL);
	virtual ~menuitem(void);

	void draw(canvas *console);

	char getKey() { return m_key; }
private:
	char m_key;
	char m_label[MAX_MENUITEM_LENGTH];
	unsigned int m_label_len;
};

}
