#include "uimenu.h"

namespace ui {

menu::menu(widget *parent)
{
	m_selected = this;
}


menu::~menu(void)
{
}

void menu::draw(canvas *console)
{
}

void menu::addItem(menuitem *item)
{
	struct menu_map d;

	d.ismenuitem = true;
	d.ptr = (void *)item;
	d.key = item->getKey();

	m_map.push_back(d);
}

void menu::addSubMenu(menu *menu, int key)
{
	struct menu_map d;

	d.ismenuitem = false;
	d.ptr = (void *)menu;
	d.key = key;

	m_map.push_back(d);
}

}