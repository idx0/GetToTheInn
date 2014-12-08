#include <algorithm>

#include "uiwidget.h"

namespace ui {

canvas::canvas(TCODConsole* console) : m_console(console)
{
}

canvas::~canvas()
{
}

void canvas::draw(const Point& p, int c, const Color& fg, const Color& bg)
{
	m_console->setChar(p.x(), p.y(), c);
	m_console->setCharForeground(p.x(), p.y(), fg.toTCOD());
	m_console->setCharBackground(p.x(), p.y(), bg.toTCOD());
}

void canvas::draw(const Point& p, int c, const Color& fg)
{
	m_console->setChar(p.x(), p.y(), c);
	m_console->setCharForeground(p.x(), p.y(), fg.toTCOD());
}

void canvas::drawText(const Rect& bounds, const std::string& sz, const Color& fg)
{
	int w = std::min(bounds.width(), (int)sz.length());
	int i = 0, j = 0;
	int cnt = 0;
	
	while (cnt < w) {
		int c = sz[cnt];

		if (c == '\n') {
			j++;
			i = 0;
			if (j >= bounds.height()) break;
		} else {
			draw(Point(i + bounds.left(), bounds.top() + j), c, fg);
			i++;
		}

		cnt++;
	}
}

canvas& canvas::operator=(const canvas& rhs)
{
	if (this != &rhs) {
		m_console = rhs.m_console;
	}

	return *this;
}

///////////////////////////////////////////////////////////////////////////////

action::action(const Key& key, const std::string& sz) : m_keycode(key), m_text(sz)
{
}

action::~action()
{
}

void action::trigger()
{
	trigger_list::iterator it = m_func.begin();

	while (it != m_func.end()) {

		// this looks wierd, but it's just us calling the function
		(*it)();

		it++;
	}
}

action& action::operator=(const action& rhs)
{
	if (this != &rhs) {
		m_func		= rhs.m_func;
		m_text		= rhs.m_text;
		m_keycode	= rhs.m_keycode;
	}

	return *this;
}

void action::addTrigger(trigger_func func)
{
	m_func.insert(func);
}

void action::removeTrigger(trigger_func func)
{
	trigger_list::iterator it = m_func.find(func);

	if (it != m_func.end()) {
		m_func.erase(it);
	}
}

void action::draw(canvas *console, const Point& tl)
{
	// TODO
	// std::string output = m_keycode.toString() + ": " + m_text;
}

///////////////////////////////////////////////////////////////////////////////

widget::widget(widget *parent) :
	m_visible(true),
	m_child(NULL),
	m_parent(parent)
{
}


widget::~widget(void)
{
	delete m_child;
}

void widget::addChild(widget *child)
{
	if (child != NULL) {
		m_child = child;
		m_child->setSize(getSize());
	}
}

void widget::draw(canvas* console)
{
	if (isVisible()) {
		drawChildren(console);
	}
}

void widget::drawChildren(canvas* console)
{
	if (m_child) {
		m_child->draw(console);
	}
}

Rect widget::getSize() const
{
	return m_size;
}

void widget::redrawNotify()
{
	if (m_parent != NULL) {
		m_parent->redrawNotify();
	}
}

void widget::show()
{
	m_visible = true;
	redrawNotify();
}

void widget::hide()
{
	m_visible = false;
	redrawNotify();
}


noop::noop(widget *parent) : widget(parent)
{
}

noop::~noop()
{
}


policy::policy()
	: m_vsize(0),
	  m_hsize(0),
	  m_vrule(POLICY_IGNORED),
	  m_hrule(POLICY_IGNORED)
{
}

policy::~policy()
{
}

policy::policy_rule policy::getRule(bool ishorizontal)
{
	policy_rule p = m_vrule;

	if (ishorizontal) {
		p = m_hrule;
	}

	return p;
}

int policy::getSize(bool ishorizontal)
{
	int s = m_vsize;

	if (ishorizontal) {
		s = m_hsize;
	}

	return s;
}

policy &policy::operator= (const policy &left)
{
	m_vsize = left.m_vsize;
	m_hsize = left.m_hsize;
	m_vrule = left.m_vrule;
	m_hrule = left.m_hrule;

	return *this;
}

}
