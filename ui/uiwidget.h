#pragma once
#include <stdio.h>

#include "../sys/logger.h"
#include "../geometry.h"
#include "../key.h"
#include "../common.h"

#include <libtcod.hpp>
#include <vector>
#include <set>

namespace ui {


class canvas
{
public:
	explicit canvas(TCODConsole* console);
	virtual ~canvas();

	virtual void draw(const Point& p, int c, const Color& fg, const Color& bg);
	virtual void draw(const Point& p, int c, const Color& fg);

	virtual void drawText(const Rect& bounds, const std::string& sz, const Color& fg);

	TCODConsole* console() { return m_console; }

	canvas& operator=(const canvas& rhs);

protected:

	TCODConsole* m_console;
};

/* This class defines the sizing policy for a widget.
 * A Sizing policy defines how the widget is stretched or resized when put in
 * a layout with other widgets.  This class sets rules for horizontal and vertical
 * stretching within a layout.  A size parameter can be specified for fixed, minimum,
 * and maximum policies.
 */
class policy
{
public:
	enum policy_rule {
		POLICY_FIXED = 0,
		POLICY_MINIMUM,
		POLICY_MAXIMUM,
		POLICY_IGNORED
	};

	explicit policy();
	virtual ~policy();

	void setHorizontalRule(policy_rule rule) { m_hrule = rule; }
	void setHorizontalSize(int size) { m_hsize = size; }
	void setVerticalRule(policy_rule rule) { m_vrule = rule; }
	void setVerticalSize(int size) { m_vsize = size; }

	policy_rule getRule(bool ishorizontal);
	int getSize(bool ishorizontal);

	policy &operator= (const policy &left);
private:
	int m_vsize;
	int m_hsize;
	policy_rule m_vrule;
	policy_rule m_hrule;
};


class widget_operator
{
public:
	widget_operator() {}
	virtual ~widget_operator() {}

	virtual void trigger() {}
	virtual void click() {}
	virtual void mouseenter(const Point& p) {}
	virtual void mouseleave(const Point& p) {}
	virtual void mousemove(const Point& p) {}
};


class action : public widget_operator
{
public:
	typedef void (*trigger_func)(void);

public:
	action(const Key& key, const std::string& sz);
	~action();

	void trigger();

	action& operator=(const action& rhs);

	void addTrigger(trigger_func func);
	void removeTrigger(trigger_func func);

	void draw(canvas *console, const Point& tl);

protected:

	Key m_keycode;
	std::string m_text;

	typedef std::set<trigger_func> trigger_list;

	trigger_list m_func;
};


class widget
{
public:
	explicit widget(widget *parent = NULL);
	virtual ~widget(void);

	widget *getParent() { return m_parent; }

	virtual void addChild(widget *child);

	virtual void draw(canvas* console);
	virtual void drawChildren(canvas* console);

	virtual Rect getSize() const;
	virtual void setSize(const Rect &size) { m_size = size; }

	void show();
	void hide();
	bool isVisible() const { return m_visible; }

	void setPolicy(const policy &p) { m_policy = p; }
	policy getPolicy() const { return m_policy; }

	virtual void redrawNotify();
protected:
	Rect m_size;
	bool m_visible;
	widget *m_child;
	policy m_policy;

private:
	widget *m_parent;
};


class noop : public widget
{
public:
	explicit noop(widget *parent = NULL);
	virtual ~noop();

	void draw() { }
	void addChild(widget *child) { ((void *)child); }
	void drawChildren() { }
};

}
