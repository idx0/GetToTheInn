#include "context.h"

#include "sys/logger.h"

Context::Context(Player *p) :
	m_player(p),
	m_viewport(NULL),
	m_console(NULL),
	m_grid(NULL),
	m_render(NULL),
	m_ready(false)
{
	sys::mutex_init(&m_mutex);
}

Context::~Context()
{
	sys::mutex_destroy(&m_mutex);

	reset();
}


void Grid::update()
{
	Object* obj = dynObj;

	if (Utils::isValid(obj)) {
		obj->update();
	} else {
		obj = mapObj;
	}

	render->tile = obj->tile();
	render->lighting = obj->lightingModel();
	render->mobility = obj->mobilityModel();

	render->lighting.reset();
	render->mobility.reset();
	render->discover.reset();
}


void Context::update()
{
	Rect r = m_viewport->render();

	// update objects, creating an accurate lighting map
	for (int x = r.left(); x < r.width(); x++) {
		for (int y = r.top(); y < r.height(); y++) {
			m_grid->at(x, y)->update();
		}
	}

	m_ready = true;
}

void Context::place(Object* obj)
{
	if (Utils::isValid(obj)) {
		m_grid->at(obj->coords())->dynObj = obj;
	}
}

void Context::lock()
{
	sys::mutex_lock(&m_mutex);
}

void Context::unlock()
{
	sys::mutex_unlock(&m_mutex);
}

void Context::reset()
{
	delete m_viewport;
	delete m_grid;
	delete m_render;

	delete m_console;
}


void Context::initialize(Map* m)
{
	const Size window(WINDOW_WIDTH, WINDOW_HEIGHT);
	const Size container(m->width(), m->height());

	// reset the context
	reset();

	m_grid = new ModelList<Grid>(m->width(), m->height());
	m_console = new TCODConsole(window.width(), window.height());

	m_render = new RenderSettings(m->width(), m->height());

	// create viewport
	m_viewport = new Viewport(container, window, 25,
							  m_player->coords().x(), m_player->coords().y());

	// set map related variables
	for (int x = 0; x < m->width(); x++) {
		for (int y = 0; y < m->height(); y++) {
			Grid *g = m_grid->at(x, y);

			g->render = m_render->at(x, y);

			// update map objects
			g->mapObj = m->staticObject(x, y);
			g->render->mobility = g->mapObj->mobilityModel();
		}
	}
}

// if we can copy do so, otherwise return immediately
void Context::trycopy(RenderSettings *render, Point* playerPos, Tile* playerTile)
{
	if (sys::mutex_trylock(&m_mutex) == 0) {
		if (m_ready) {
			render->copy(m_render, m_viewport->viewport());

			*playerPos = m_player->coords() - m_viewport->viewport().topLeft();
			*playerTile = m_player->tile();

			m_ready = false;
		}
		sys::mutex_unlock(&m_mutex);
	}
}


Object* Context::objAt(int x, int y)
{
	if (m_grid->inbounds(x, y)) {

		Object* obj = m_grid->at(x, y)->dynObj;

		if (!Utils::isValid(obj)) {
			obj = m_grid->at(x, y)->mapObj;
		}

		return obj;
	}
	return static_cast<Object*>(0);
}

Object* Context::objAt(const Point& p)
{
	return objAt(p.x(), p.y());
}


void Context::discover(int x, int y)
{
}

Cursor Context::cursor()
{
	Cursor ret;

	lock();
	ret = m_cursor;
	unlock();

	return ret;
}

void Context::updateCursor(const Point& pos, bool visible)
{
	Cursor cur;
	cur.visible = visible;
	cur.pos		= pos;

	lock();
	m_cursor = cur;
	unlock();
}