#include "engine.h"
#include "key.h"
#include "animation.h"
#include "rnd.h"

#include "pathfinding.h"
#include "sys/eventqueue.h"
#include "sys/logger.h"

#include "raylib.h"
#include <sstream>

struct _mouse
{
	bool pressed;
	Point p;
};

static _mouse _ms;


///////////////////////////////////////////////////////////////////////////////

UpdateThread::UpdateThread(Context* ctx, Player* player) :
	sys::thread(THREAD_JOINABLE),
	m_context(ctx), m_player(player), m_done(false)
{
	m_listener->addListener(sys::EVENT_UPDATE, ev_update);
	m_listener->addListener(sys::EVENT_ACTION, ev_action);
	m_listener->addListener(sys::EVENT_LIGHTING, ev_lighting);
}

UpdateThread::~UpdateThread()
{
}

void UpdateThread::thread_func()
{
	while (!isDone()) {
		m_listener->pop();
	}
}

void UpdateThread::ev_update(void *tag, const sys::event_payload::_data& data)
{
	UpdateThread* ut = (UpdateThread*)tag;

	if (ut) {
		ut->block();
		ut->update();
		ut->unblock();
	}
}

void UpdateThread::ev_lighting(void *tag, const sys::event_payload::_data& data)
{
	UpdateThread* ut = (UpdateThread*)tag;

	if (ut) {
		ut->block();
		ut->lighting();
		ut->unblock();
	}
}

void UpdateThread::ev_action(void *tag, const sys::event_payload::_data& data)
{
	UpdateThread* ut = (UpdateThread*)tag;

	if (ut) {
		ut->block();

		if (data.flags & A_MOVE) {
			int kc = (int)data.param1;

			// move player - we intend to move, so move
			if (ut->m_player->move(NEIGHBORS[kc].dx, NEIGHBORS[kc].dy, ut->m_context->grid())) {
				ut->m_context->viewport()->scroll(NEIGHBORS[kc].dx, NEIGHBORS[kc].dy);
			}
		}

		ut->update();
		ut->unblock();
	}
}

void UpdateThread::block()
{
	m_mode = Engine::mode();
	Engine::waitingMode();

	m_context->lock();
}

void UpdateThread::unblock()
{
	m_context->unlock();

	Engine::restoreMode(m_mode);
}

void UpdateThread::lighting()
{
	LightingEngine::getInstance()->calculateLighting(m_context->grid(),
													 m_context->viewport()->render());
}

void UpdateThread::update()
{
#if 0
	// lock down the context
	m_context->lock();

	// move player - we intend to move, so move
	if (m_player->move(NEIGHBORS[kc].dx, NEIGHBORS[kc].dy, m_context->grid())) {
		m_context->viewport()->scroll(NEIGHBORS[kc].dx, NEIGHBORS[kc].dy);
	}
#endif

	// update context
	m_context->update();

//	if (m_emitter) { m_emitter->update(); }

	// calculate lighting
	lighting();

	// update player
	m_player->update(m_context->grid());

#if 0
	// unlock the context
	m_context->unlock();
#endif
    printf("EE: updated\n");
}

void UpdateThread::done(bool d)
{
	lock();
	m_done = d;
	unlock();
}

bool UpdateThread::isDone()
{
	bool d = false;

	lock();
	d = m_done;
	unlock();

	return d;
}

///////////////////////////////////////////////////////////////////////////////

Engine *Engine::e = static_cast<Engine*>(0);

Engine::Engine() :
    m_mode(MODE_MOVE),
	m_updateNeeded(true), m_quit(false)
{
    printf("EE: begin!\n");
}

void Engine::init()
{
	e = getInstance();

    printf("EE: creating TileEngine...\n");
    e->m_engine = new TileEngine("ascii.png", 10, 10, WINDOW_WIDTH, WINDOW_HEIGHT + STATUS_HEIGHT);
    printf("EE: done.\n");

	// init game stuff
	e->m_map = new Map(MAP_WIDTH, MAP_HEIGHT);

	// generate the map
	e->m_map->generateMap();

	// create player
	Point p = e->m_map->findNear(Point(41, 25), 4);
	e->m_player = new Player(p.x(), p.y());

	// create context;
	e->m_context = new Context(e->m_engine->mainConsole(), e->m_player);
	e->m_context->initialize(e->m_map);

    printf("EE: Initialied console %p\n", e->m_engine->mainConsole());

	// place objects
//	e->m_context->place(new PhantomWall(41, 22));
    MagicTree *tree = new MagicTree(e->m_map->findFree(4));
    e->m_map->overgrow(tree->coords().x(), tree->coords().y(), 12);
	e->m_context->place(tree);

	for (int i = 0; i < 10; i++) {
        MagicShroom *shroom = new MagicShroom(e->m_map->findFree(3));
        e->m_map->overgrow(shroom->coords().x(), shroom->coords().y(), 4);
        e->m_context->place(shroom);
	}

    e->m_context->updateMap(e->m_map);
    e->m_player->update(e->m_context->grid());

	e->m_updateThread = new UpdateThread(e->m_context, e->m_player);
	e->m_renderThread = new RenderThread(e->m_engine, e->m_context);
	e->m_uiThread = new ui::uithread();

	// create some UI stuff
	e->intiui();

#ifdef THREADS
	// create update thread
	e->m_updateThread->create_thread();

	// force an update
	sys::event_payload::_data data;
	data.param1 = NNEIGHBORS;

	UpdateThread::ev_update(e->m_updateThread, data);

	// create render thread
    e->m_renderThread->create_thread();
#endif
}

void Engine::run()
{
    e->m_engine->run();
}

void Engine::intiui()
{
	Size s = m_context->viewport()->viewport().size();
	int ptop = s.height();

	ui::panel* p0 = new ui::panel(gtti::Color(32,32,32));
	e->m_modeLabel = new ui::label("-- waiting --");

	ui::panel* p1 = new ui::panel(gtti::Color(32,32,32));
	m_flavorLabel = new ui::label("");
	m_flavorLabel->setTextColor(gtti::Color::grey);

	p0->setSize(Rect(ptop, 0, ptop + 1, s.width()));
	p0->addChild(e->m_modeLabel);

	p1->setSize(Rect(ptop + 1, 0, ptop + 2, s.width()));
	p1->addChild(e->m_flavorLabel);

	m_messageLabel = new ui::label("The floor beneath you is made of large stone slabs.  They look well worn -- and much neglected -- a relic of civiliza- tions past.  A mossy fungus seems to grow between the cracks.");
	m_messageWindow = new ui::panel();

	m_messageWindow->setSize(Rect(5, 46, 21, 91));	// position 1
	m_messageWindow->setSize(Rect(52, 46, 68, 91));	// position 2
	m_messageWindow->setSize(Rect(52, 4, 68, 49));	// poisiton 3

	ui::frame *frame = new ui::frame(ui::FRAME_TYPE_THIN);
	frame->setColor(gtti::Color(208, 208, 208));
	frame->addChild(m_messageLabel);

	m_messageWindow->addChild(frame);
	m_messageWindow->hide();

    ui::vbox *b = new ui::vbox(Rect(ptop, 0, ptop + STATUS_HEIGHT, s.width()));
    b->addChild(new ui::block(196, gtti::Color(208, 208, 208), Rect(0, 0, Size(s.width(), 1))));
    b->addChild(p0);
    b->addChild(p1);

    m_uiThread->own(b);
	m_uiThread->own(m_messageWindow);
}

void Engine::final()
{
#ifdef THREADS
	e->m_updateThread->done();
	e->m_renderThread->done();

	sys::thread_join(e->m_updateThread, NULL);
	sys::thread_join(e->m_renderThread, NULL);
#endif
}

Engine::~Engine()
{
    printf("EE: end!\n");
    delete m_engine;
	delete m_map;
	delete m_player;
	delete m_context;
}

bool Engine::quit()
{
	return e->m_quit;
}

InputMode Engine::mode()
{
	return e->m_mode;
}

ui::uithread* Engine::theUI()
{
	return e->m_uiThread;
}

void Engine::drawGround(int x, int y, bool visible, TemperatureModel* tm) const
{
	if (!visible) return;

	bool cold = (tm->temp < 0);
	int abs_temp = abs(tm->temp);

	gtti::Color bg = gtti::Color::black;

	static float r = Rnd::rndn();

	if (cold) {
		if (abs_temp >= 7) {
			bg = gtti::Color::lerp(gtti::Color(0, 63, 127), gtti::Color(0, 31, 63), r);
		} else if (abs_temp >= 4) {
			bg = gtti::Color::lerp(gtti::Color(0, 95, 127), gtti::Color(0, 47, 63), r);
		}
	} else {
		if (abs_temp >= 7) {
			bg = gtti::Color::lerp(gtti::Color(127, 31, 0), gtti::Color(63, 15, 0), r);
		} else if (abs_temp >= 4) {
			bg = gtti::Color::lerp(gtti::Color(127, 63, 0), gtti::Color(63, 31, 0), r);
		}
	}

//	m_groundEffects->setCharBackground(x, y, bg);
}

// Bresenham's line algorithm
// http://members.chello.at/~easyfilter/bresenham.html
void Engine::drawLine(const Point& p0, const Point& p1)
{
	int x = p0.x(), y = p0.y();
	int dx =  abs(p1.x() - x), sx = (x < p1.x() ? 1 : -1);
	int dy = -abs(p1.y() - y), sy = (y < p1.y() ? 1 : -1);
	int err = dx + dy, e2;

	for (;;) {
		// draw(x, y);
//		m_mapEffects->setCharBackground(x, y, TCODColor::magenta);

		if (!(m_context->grid()->get(x, y)->render->mobility.flags & M_WALKABLE)) break;
		if ((x == p1.x()) && (y == p1.y())) break;

		e2 = 2 * err;
		if (e2 >= dy) { err += dy; x += sx; }
		if (e2 <= dx) { err += dx; y += sy; }
	}
}

// Bresenham's circle algorithm
void Engine::drawCircle(const Point& o, int r)
{
	int x = -r, y = 0, err = 2 - 2 * r;

	do {
//		m_mapEffects->setCharBackground(o.x() - x, o.y() + y, TCODColor::purple);
//		m_mapEffects->setCharBackground(o.x() - y, o.y() - x, TCODColor::purple);
//		m_mapEffects->setCharBackground(o.x() + x, o.y() - y, TCODColor::purple);
//		m_mapEffects->setCharBackground(o.x() + y, o.y() + x, TCODColor::purple);

		r = err;
		if (r <= y) err += ++y * 2 + 1;
		if ((r > x) || (err > y)) err += ++x * 2 + 1;
	} while (x < 0);
}

void Engine::render()
{
#if 0
	sys::event_payload pl;

	pl.ptr = NULL;
	pl.token = sys::token(sys::TOKEN_EVENT, 1).getObjectId();

	// push a render event
	sys::eventqueue::push(sys::event(sys::EVENT_RENDER, pl));
#else
    RenderThread::ev_render(e->m_renderThread, sys::TOKEN_NONE);
#endif
}


void Engine::update(int kc)
{

	sys::event_payload pl;

//	pl.ptr = // TODO key input
	pl.data.param1 = (unsigned short)kc;

//	sys::logger::log("update triggered via %d", kc);
#if 0
	// push a render event
	sys::eventqueue::push(sys::event(sys::EVENT_UPDATE, pl));
#else
    UpdateThread::ev_update(e->m_updateThread, pl.data);
#endif
}

void Engine::waitingMode()
{
	e->m_mode = MODE_WAITING;

	e->m_uiThread->lock();
	e->m_modeLabel->setLabel("-- waiting --");
	e->m_uiThread->unlock();
}

void Engine::restoreMode(const InputMode& m)
{
	e->m_mode = m;

	e->m_uiThread->lock();
	switch (m) {
	default:
	case MODE_WAITING:
		e->m_modeLabel->setLabel("-- waiting --");
		break;
	case MODE_MOVE:
		e->m_modeLabel->setLabel("MOVE");
		break;
	case MODE_COMBAT:
		e->m_modeLabel->setLabel("COMBAT");
		break;
	case MODE_INSPECT:
		e->m_modeLabel->setLabel("INSPECT");
		break;
	case MODE_STEALTH:
		e->m_modeLabel->setLabel("STEALTH");
		break;
	}
	e->m_uiThread->unlock();
}

bool Engine::modeNeedsCursor(const InputMode& m) const
{
	return ((m == MODE_COMBAT) || (m == MODE_INSPECT));
}

void Engine::checkForInput()
{
    int kc = NNEIGHBORS;
#if 0
	static const int FREQ = (1000 / FPS) + 1;

	TCOD_event_t ev = TCODSystem::checkForEvent(TCOD_EVENT_ANY, &e->m_key, &e->m_mouse);

	if (ev == TCOD_EVENT_KEY_PRESS) {
		kc = handleKeyDown(e->m_key);
	}

	Point mp(e->m_mouse.cx, e->m_mouse.cy);

	if (ev == TCOD_EVENT_MOUSE_MOVE) {
		sys::event_payload p;
		p.data.param1 = e->m_mouse.cx;
		p.data.param2 = e->m_mouse.cy;

		sys::eventqueue::push(sys::event(sys::EVENT_MOUSE_MOVE, p));
	}
#endif

	Cursor c = e->m_context->cursor();

	bool vis = c.visible;
	Point mp = c.pos;

	// does this mode require a cursor, and is the cursor on the map?
	if ((e->modeNeedsCursor(e->m_mode)) &&
		(e->m_context->viewport()->viewport().inbounds(mp))) {
        if (!e->m_shownCursor) {
            mp = e->m_player->coords();
            e->m_shownCursor = true;
        }
		vis = true;
    } else {
        vis = false;
    }

    int k = GetKeyPressed();
    kc = handleKeyDown(k);

    if (kc != NNEIGHBORS) {
        if (e->m_mode == MODE_MOVE) {
            sys::event_payload pl;

            //		pl.ptr = // TODO key input
            pl.data.param1 = (unsigned short)kc;
            pl.data.flags = A_MOVE;

            //		sys::logger::log("update triggered via %d", kc);

                    // push an update event
            //sys::eventqueue::push(sys::event(sys::EVENT_ACTION, pl));
            UpdateThread::ev_action(e->m_updateThread, pl.data);
        } else if (vis) {
            Point mv(NEIGHBORS[kc].dx, NEIGHBORS[kc].dy);

            if (IsKeyPressed(KEY_LEFT_SHIFT) || IsKeyPressed(KEY_RIGHT_SHIFT)) {
                mv *= 10;
            }

            if (e->m_context->viewport()->viewport().inbounds(mp + mv)) {
                mp += mv;
            }
        }
	} else if (k == 'z') {
        mp = e->m_player->coords();
    }

	if (vis) {
		// describe the object at the current cursor position
		Object* obj = e->m_context->objAt(mp);
		TheGrid* g = e->m_context->grid();

		e->m_uiThread->lock();
		if ((obj) && (g->get(mp)->render->discover.flags & D_EXPLORED)) {
			e->m_flavorLabel->setLabel(obj->flavor());
		} else {
			e->m_flavorLabel->setLabel("");
		}
		e->m_uiThread->unlock();
	}

	e->m_context->updateCursor(mp, vis);

#if 0
	TCODSystem::sleepMilli(FREQ);

	// this call seems to smooth things out, but im worried about FPS limiting
	// my guess is this works because it forces the renderer to redraw after every event
	// but we would rather let the render thread handle this
//	TCODConsole::flush();
#endif
}


int Engine::handleKeyDown(int key)
{

	int kc = NNEIGHBORS;
	static int windowPos = 0;
    bool needsUpdate = false;

#if 0
	if (key.pressed == 0) return kc;

	switch (key.vk) {
#endif

    if (key) printf("key pressed %d\n", key);

    switch ((KeyboardKey)key) {
		case KEY_ESCAPE:
        case 'q':
			e->m_quit = true;
			break;
		case 'i':
		{
			int px = e->m_player->coords().x();
			int py = e->m_player->coords().y();

			for (int n = 0; n < NNEIGHBORS; n++) {
				Object *obj = e->m_context->objAt(NEIGHBORS[n].dx + px, NEIGHBORS[n].dy + py);
				obj->interact();
			}
            break;
		}
        case 'x':
        {
			e->m_uiThread->lock();
			e->m_modeLabel->setLabel("INSPECT");
			e->m_uiThread->unlock();
			e->m_mode = MODE_COMBAT;
			break;
		}
		case KEY_SPACE:
		{
			InputMode nmode = e->m_mode;

			e->m_uiThread->lock();
			switch (e->m_mode) {
			default: break;
			case MODE_MOVE:
				nmode = MODE_COMBAT;
				e->m_modeLabel->setLabel("COMBAT");
				break;
			case MODE_COMBAT:
				nmode = MODE_MOVE;
				e->m_modeLabel->setLabel("MOVE");
				break;
			}
			e->m_uiThread->unlock();
			e->m_mode = nmode;

			break;
		}
        case 't':
        {
            Torch *t = new Torch(e->m_player->coords().x(), e->m_player->coords().y(), 3, 5);
            if (!e->m_context->place(t)) {
                delete t;
            } else {
                needsUpdate = true;
            }
            break;
        }
        case 'e':
        {
            NamedObject *obj = e->m_context->pickup(Point(e->m_player->coords().x(), e->m_player->coords().y()));
            if (!obj) {
                obj = e->m_context->pickup(e->m_player->inFrontOf());
            }

            if (obj) {
                e->m_flavorLabel->setLabel("You picked up a " + obj->name());
                if (dynamic_cast<MagicShroom*>(obj)) {
                    e->m_engine->mainConsole()->setFilter(new PsychedlicFilter());
                }
                delete obj;
                needsUpdate = true;
            } else {
                e->m_flavorLabel->setLabel("There is nothing to be picked up!");
            }
            break;
        }
        case 'f':
            e->m_engine->mainConsole()->setFilter(new FadeFilter(0.1f, 0.0f, 0.5f, gtti::Color::white));
            break;
        case 'k':
            e->m_engine->mainConsole()->setFilter(new FadeFilter(0.05f, 0.0f, 0.2f, gtti::Color(255, 96, 96)));
            break;
        case 'l':
            e->m_engine->mainConsole()->setFilter(new ThunderstormFilter());
            break;
		case KEY_TAB:
        case 'm':
		{
			windowPos = (windowPos + 1) % 4;

			e->m_uiThread->lock();
			switch (windowPos) {
			case 0: e->m_messageWindow->hide(); break;
			case 1:
				e->m_messageWindow->show();
				e->m_messageWindow->setSize(Rect(5, 46, 21, 91));	// position 1
				break;
			case 2:
				e->m_messageWindow->setSize(Rect(52, 46, 68, 91));	// position 2
				break;
			case 3:
				e->m_messageWindow->setSize(Rect(52, 4, 68, 49));	// poisiton 3
				break;
			}
			e->m_uiThread->unlock();
			break;
		}
#if 0
		case TCODK_PRINTSCREEN:
			TCODSystem::saveScreenshot(NULL);
			break;
#endif
		case KEY_KP_8:
		case KEY_UP:
        case KEY_W:
        case 'w':
			kc = NORTH;
			break;
		case KEY_KP_7:
			kc = NORTHWEST;
			break;
		case KEY_KP_9:
			kc = NORTHEAST;
			break;
		case KEY_KP_2:
		case KEY_DOWN:
        case KEY_S:
        case 's':
			kc = SOUTH;
			break;
		case KEY_KP_1:
			kc = SOUTHWEST;
			break;
		case KEY_KP_3:
			kc = SOUTHEAST;
			break;
		case KEY_KP_4:
		case KEY_LEFT:
        case KEY_A:
        case 'a':
			kc = WEST;
			break;
		case KEY_KP_6:
		case KEY_RIGHT:
        case KEY_D:
        case 'd':
			kc = EAST;
			break;
		default: break;
	}

#if 0
    if (IsKeyDown(KEY_KP_8) || IsKeyDown(KEY_UP)) {
        kc = NORTH;
    } else if (IsKeyDown(KEY_KP_2) || IsKeyDown(KEY_DOWN)) {
        kc = SOUTH;
    } else if (IsKeyDown(KEY_KP_4) || IsKeyDown(KEY_LEFT)) {
        kc = WEST;
    } else if (IsKeyDown(KEY_KP_6) || IsKeyDown(KEY_RIGHT)) {
        kc = EAST;
    } else if (IsKeyDown(KEY_KP_1)) {
        kc = SOUTHWEST;
    } else if (IsKeyDown(KEY_KP_3)) {
        kc = SOUTHEAST;
    } else if (IsKeyDown(KEY_KP_9)) {
        kc = NORTHEAST;
    } else if (IsKeyDown(KEY_KP_7)) {
        kc = NORTHWEST;
    }
#endif

//	sys::logger::log("key_down := %s", Key(key).toString().c_str());

    if (needsUpdate) {
        UpdateThread::ev_update(e->m_updateThread, sys::event_payload::_data{});
    }

	return kc;
}

int Engine::handleKeyUp(int key)
{
	return 0;
}
