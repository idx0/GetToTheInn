#pragma once

#include "sys/thread.h"
#include "context.h"
#include "key.h"
#include "ui/ui.h"

enum RenderMode
{
	R_NORMAL,					// this is the default
	R_TRUECOLOR,				// this render mode changes the render rules for sensed tiles so
								// all lit, visible tiles are draw in full color
	R_STEALTH,					// this render mode changes the render rules for sensed tiles in
								// order to simulate stealth
	R_FULL,						// this render mode draws all cells in their given color
	R_DEBUG_DISCOVERY,			// a debug view which shows discover calculations
	R_DEBUG_PATHING,			// a debug view which shows pathing calculations
};


class RenderThread : public sys::thread
{
public:
	RenderThread(Context* ctx);
	~RenderThread();

	void thread_func();
	bool isDone();
	void done(bool d = true);

	static void ev_render(void *tag, const sys::token_id id);

protected:

	virtual void render();

	void draw(int x, int y) const;

	void drawNormal(int x, int y) const;
	void drawTruecolor(int x, int y) const;
	void drawStealth(int x, int y) const;
	void drawFull(int x, int y) const;
	void drawDebugDiscovery(int x, int y) const;
	void drawDebugPathing(int x, int y) const;

protected:

	RenderSettings *m_render;
	TCODConsole* m_console;

	ui::canvas* m_rootCanvas;

	TCODConsole* m_cursor;

	Point m_mouse;
	Key m_key;

	Context* m_context;

	Rect m_screen;
	Point m_playerPos;
	Tile m_playerTile;

	RenderMode m_mode;

	bool m_done;
};
