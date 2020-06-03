#pragma once

#include "common.h"
#include "object.h"
#include "player.h"
#include "lighting.h"
#include "map.h"
#include "viewport.h"
#include "effects.h"

#include "sys/thread.h"

#include "raylib.h"
#include <string>

struct Cursor
{
	Cursor() : pos(0, 0), visible(false) {}
	Cursor& operator=(const Cursor& rhs)
	{
		if (this != &rhs) {
			pos			= rhs.pos;
			visible		= rhs.visible;
		}
		return *this;
	}

	Point pos;
	bool visible;
};

// The context is a list of all in-game object, items, maps, etc. that are
// relavent to the current map of the current game.  Every new map will
// have a new, unique, context.

class Context
{
public:
	Context(Console *c, Player* p);
	virtual ~Context();

	virtual void update();

	bool place(Object* obj);
    NamedObject *pickup(const Point& pos);

	// initializes the context based on the given map
	void initialize(Map* m);

	// discovers objects at x, y
	void discover(int x, int y);

	Viewport* viewport();
	Console* console();

	void lock();
	void unlock();

	Cursor cursor();
	void updateCursor(const Point& pos, bool visible = false);

    void updateMap(Map *m);

	ModelList<Grid>* grid();

	Object* objAt(int x, int y);
	Object* objAt(const Point& p);

	bool trycopy(RenderSettings *render, Point* playerPos, Tile* playerTile);

protected:

	void reset();

protected:

	Player* m_player;
	Viewport* m_viewport;

    Console* m_console;

	TheGrid *m_grid;
	RenderSettings *m_render;

	Cursor m_cursor;
	sys::mutex m_mutex;
	bool m_ready;
};

inline
Viewport* Context::viewport()
{
	return m_viewport;
}

inline
ModelList<Grid>* Context::grid()
{
	return m_grid;
}


inline
Console* Context::console()
{
	return m_console;
}
