#pragma once
#include "uiwidget.h"
#include "uimenuitem.h"

#include <vector>
using namespace std;

namespace ui {

class menu : public widget
{
public:
	menu(widget *parent = NULL);
	~menu(void);

	void addItem(menuitem *item);
	void addSubMenu(menu *menu, int key);

	virtual void draw(canvas *console);
	void drawChildren(canvas *console) { }

private:
	struct menu_map
	{
		bool ismenuitem;
		void *ptr;
		int key;
	};

private:
	menu *m_selected;

	vector<struct menu_map> m_map;
};

}
