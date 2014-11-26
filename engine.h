#pragma once

#include <list>
#include <map>
#include <memory>
#include <string>

#include "util.h"
#include "map.h"
#include "common.h"
#include "fov/fov.h"
#include "player.h"
#include "object.h"
#include "viewport.h"
#include "effects.h"

#include "ui/ui.h"
#include "sys/thread.h"
#include "context.h"
#include "render.h"


enum InputMode
{
	MODE_WAITING = 0,
	MODE_MOVE,
	MODE_COMBAT,
	MODE_INSPECT,
	MODE_STEALTH
};

// input actions are used as event flags
enum InputAction
{
	A_MOVE			= B1(0),		// action moves player
	A_ATTACK		= B1(1),		// action attack monster or object
	A_USE			= B1(2)			// action uses object
};

class UpdateThread : public sys::thread
{
public:
	UpdateThread(Context* ctx, Player* player);
	~UpdateThread();

	void thread_func();
	bool isDone();
	void done(bool d = true);

	static void ev_update(void *tag, const sys::event_payload::_data& data);
	static void ev_action(void *tag, const sys::event_payload::_data& data);
	static void ev_lighting(void *tag, const sys::event_payload::_data& data);

	void block();
	void unblock();

protected:

	virtual void lighting();
	virtual void update();

protected:

	InputMode m_mode;
	Context* m_context;
	Player* m_player;

	bool m_done;
};

class Engine : public Utils::Singleton<Engine>
{
public:
	Engine();
	~Engine();

	static void init();
	static void final();

	static void checkForInput();
	
	static bool quit();

	static ui::uithread* theUI();

	static InputMode mode();
	static void waitingMode();
	static void restoreMode(const InputMode& m);

protected:

	void intiui();

	void render();
	void update(int kc);

	void drawGround(int x, int y, bool visible, TemperatureModel* tm) const;

	void drawLine(const Point& p0, const Point& p1);
	void drawCircle(const Point& o, int r);

	Player* player() { return m_player; }

	Map* map() { return m_map; }

	static Engine *e;

	bool modeNeedsCursor(const InputMode& m) const;

protected:

	static int handleKeyDown(const TCOD_key_t& key);
	static int handleKeyUp(const TCOD_key_t& key);

protected:
	Map *m_map;
	Player *m_player;

	// 'space' - switches between MOVE and COMBAT modes ("changes stances")
	// 'x' - toggles INSPECT mode (ESC also switches back)
	// 'c' - toggles STEALTH in MOVE mode (COMBAT or saving throw will also break STEALTH)
	// WAITING mode is enabled by a call to update() and set as long as update() is calculating

	// An ACTION is any activity undertaken by the player which ends the turn.  ACTIONs always
	// call update() when finished (to end the turn)
	InputMode m_mode;

	UpdateThread* m_updateThread;
	RenderThread* m_renderThread;


	ui::uithread* m_uiThread;

	ui::label* m_flavorLabel;
	ui::label* m_modeLabel;

	ui::label* m_messageLabel;
	ui::panel* m_messageWindow;

	unsigned int m_renderFlags;

	Context* m_context;

	Emitter* m_emitter;
	TCODConsole* m_mapEffects;

	TCODConsole* m_groundEffects;
	ConstantDelay *m_groundUpdate;

	TCOD_key_t m_key;
	TCOD_mouse_t m_mouse;

	bool m_updateNeeded;
	bool m_quit;
};

