#include "render.h"
#include "engine.h"

RenderThread::RenderThread(TileEngine *eng, Context* ctx) :
	sys::thread(THREAD_JOINABLE),
	m_context(ctx),
    m_renderer(eng),
	m_mode(R_NORMAL),
	m_done(false)
{
	Size s = ctx->viewport()->viewport().size();

	m_render = new RenderSettings(s.width(), s.height());

    m_listener->addListener(sys::EVENT_RENDER, ev_render);


#if 0
    m_renderer = new TCODConsole(s.width(), s.height());

	TCODMouse::showCursor(false);
    m_cursor->setChar(0, 0, ' ');
    m_cursor->setCharBackground(0, 0, TCODColor::white);
#else
    m_rootCanvas = new ui::canvas(eng->addLayer("ui", 0, 0, eng->mainConsole()->width(), eng->mainConsole()->height()));
    m_cursor = eng->addLayer("cursor", 0, 0, 1, 1);

    printf("EE: setting up console %p\n", ctx->console());
#endif
}

RenderThread::~RenderThread()
{
	delete m_render;

	delete m_renderer;
    delete m_rootCanvas;

	//delete m_cursor;
}

void RenderThread::ev_render(void *tag, const sys::token_id id)
{
	RenderThread* rt = (RenderThread*)tag;

	if (rt) rt->render();
}

void RenderThread::render()
{
    if (m_context->trycopy(m_render, &m_playerPos, &m_playerTile)) {
        return;
    }

    //	m_renderer->clear();

    // render player's view
	for (int x = 0; x < m_render->width(); x++) {
		for (int y = 0; y < m_render->height(); y++) {
			draw(x, y);
//			m_render->at(x, y)->lighting.reset();
//			m_render->at(x, y)->discover.reset();
		}
	}

	// render player
	int px = m_playerPos.x();
	int py = m_playerPos.y();

	m_renderer->setChar(px, py, m_playerTile.icon);
	m_renderer->setCharForeground(px, py, m_playerTile.fgColor.toColor());

	// blit the console
//	TCODConsole::blit(m_renderer, 0, 0, 0, 0, TCODConsole::root, 0, HEADER_SPACE);
	
	// render UI
	Engine::theUI()->render(m_rootCanvas);

#ifdef USE_CURSOR
	// -- cursor --
	lock();
	Point m = m_mouse;
	unlock();

	if (m_render->inbounds(m - Point(0, HEADER_SPACE))) {
		TCODConsole::blit(m_cursor, 0, 0, 0, 0,
						  TCODConsole::root,
						  m.x(), m.y(),
						  0.0f, 0.25f);
	}
#endif

	Cursor c = m_context->cursor();
	if (c.visible) {
        Color bg = WHITE;
        bg.a = 64;

        m_cursor->show();
        m_cursor->setPosition(c.pos);
        m_cursor->setCharBackground(0, 0, bg);
    } else {
        m_cursor->hide();
    }

    BeginDrawing();
        ClearBackground(BLACK);
        m_renderer->draw();
    EndDrawing();
}

void RenderThread::thread_func()
{
	while (!isDone()) {
		m_listener->pop();

		render();
	}
}

void RenderThread::draw(int x, int y) const
{
	switch (m_mode) {
	default:
	case R_NORMAL:
		drawNormal(x, y);
		break;
	case R_TRUECOLOR:
		drawTruecolor(x, y);
		break;
	case R_STEALTH:
		drawStealth(x, y);
		break;
	case R_FULL:
		drawFull(x, y);
		break;
	case R_DEBUG_DISCOVERY:
		drawDebugDiscovery(x, y);
		break;
	case R_DEBUG_PATHING:
		drawDebugPathing(x, y);
		break;
	}
}

void RenderThread::drawNormal(int x, int y) const
{
	static const float sAMBIENT_MAX = 0.75f;
	static const float sAMBIENT_MIN = 0.35f;

    gtti::Color fg = m_render->get(x, y)->tile.fgColor;
    gtti::Color bg = m_render->get(x, y)->tile.bgColor;
    gtti::Color dbg = bg, dfg = fg;	// dxg - saturated color (varies with ambient color)
    gtti::Color fog = fg;				// fog-of-war color

	float ac = m_render->get(x, y)->lighting.ambientColor.average();

	// some ambient (dark, but visible) value - scaled by tile ambient light
	float sat = std::max(sAMBIENT_MIN, std::min(sAMBIENT_MAX, ac / (float)255));

	int icon = m_render->get(x, y)->tile.icon;

#if 0
	// if there is no background color, and either the the icon is a space (so the fg color
	// doesnt matter) or there is no foreground color, then just return because there is nothing
	// to draw.
	if ((bg == gtti::Color::black) && ((icon == ' ') || (fg == gtti::Color::black))) return;
#endif

	// draw char
	m_renderer->setChar(x, y, icon);

	dfg.darken(sat);
	dbg.darken(sat);
	fog.darken(sat - sAMBIENT_MIN);

    gtti::Color lc = m_render->get(x, y)->lighting.lightColor;
	lc.smooth();

	if (m_render->get(x, y)->discover.flags & D_SEEN) {
		if (m_render->get(x, y)->lighting.flags & L_LIT) {
#if 1
			// if visible, draw at the given light level
            gtti::Color lfg = gtti::Color::multiply(fg, lc);
            gtti::Color lbg = gtti::Color::multiply(bg, lc);
			lfg.clamp(dfg, gtti::Color::white);
			lbg.clamp(dbg, gtti::Color::white);
#else
            gtti::Color lfg = gtti::Color::blend
#endif

			m_renderer->setCharForeground(x, y, lfg.toColor());
			m_renderer->setCharBackground(x, y, lbg.toColor());
		} else if (m_render->get(x, y)->discover.flags & D_EXPLORED) {
			// explored, but in the dark
			m_renderer->setCharForeground(x, y, dfg.toColor());
			m_renderer->setCharBackground(x, y, dbg.toColor());
		} else {
			// seen it, but not explored
			m_renderer->setCharForeground(x, y, gtti::Color::black.toColor());
			m_renderer->setCharBackground(x, y, gtti::Color::black.toColor());
		}

		if ((m_render->get(x, y)->discover.flags & D_EXPLORED) &&
			(m_render->get(x, y)->lighting.flags & L_ALWAYS_LIT)) {
			m_renderer->setCharForeground(x, y, fg.toColor());
		}
	} else if (m_render->get(x, y)->discover.flags & D_EXPLORED) {
		if ((m_render->get(x, y)->lighting.flags & L_TRANSPARENT) &&
			(!(m_render->get(x, y)->lighting.flags & L_ALWAYS_LIT))) {
			// if transparent, but not a full-if-visible token just color with
			// a really dark fog of war
			m_renderer->setCharForeground(x, y, fog.toColor());
			m_renderer->setCharBackground(x, y, fog.toColor());
		} else {
			// otherwise, saturate
			m_renderer->setCharForeground(x, y, dfg.toColor());
			m_renderer->setCharBackground(x, y, dbg.toColor());
		}

//		m_renderer->setCharBackground(x, y, TCODColor::black);
	} else {
		// not seen, black
		m_renderer->setCharForeground(x, y, gtti::Color::black.toColor());
		m_renderer->setCharBackground(x, y, gtti::Color::black.toColor());
	}
}

void RenderThread::drawTruecolor(int x, int y) const
{
    gtti::Color fg = m_render->get(x, y)->tile.fgColor;
    gtti::Color bg = m_render->get(x, y)->tile.bgColor;

    gtti::Color lc = m_render->get(x, y)->lighting.lightColor;
	lc.smooth();

//	int llevel = (lc.r() + lc.b() + lc.g());

	int icon = m_render->get(x, y)->tile.icon;

	// if there is no background color, and either the the icon is a space (so the fg color
	// doesnt matter) or there is no foreground color, then just return because there is nothing
	// to draw.
	if ((bg == gtti::Color::black) && ((icon == ' ') || (fg == gtti::Color::black))) return;

	// draw char
	m_renderer->setChar(x, y, icon);

	if (m_render->get(x, y)->discover.flags & D_SEEN) {
		m_renderer->setCharForeground(x, y, fg.toColor());
		m_renderer->setCharBackground(x, y, bg.toColor());
	} else if (m_render->get(x, y)->discover.flags & D_EXPLORED) {
		m_renderer->setCharForeground(x, y, gtti::Color(31, 31, 31).toColor());
		m_renderer->setCharBackground(x, y, gtti::Color::black.toColor());
	} else {
		m_renderer->setCharForeground(x, y, gtti::Color::black.toColor());
		m_renderer->setCharBackground(x, y, gtti::Color::black.toColor());
	}
}

void RenderThread::drawFull(int x, int y) const
{
    gtti::Color fg = m_render->get(x, y)->tile.fgColor;
    gtti::Color bg = m_render->get(x, y)->tile.bgColor;
	int icon = m_render->get(x, y)->tile.icon;

	m_renderer->setChar(x, y, icon);
	m_renderer->setCharForeground(x, y, fg.toColor());
	m_renderer->setCharBackground(x, y, bg.toColor());
}

void RenderThread::drawStealth(int x, int y) const
{
}

void RenderThread::drawDebugDiscovery(int x, int y) const
{
}

void RenderThread::drawDebugPathing(int x, int y) const
{
#if 0
	static const short sMAX_DEBUG_PATH_LEN = 15;
	short val = *g_distMap.at(x, y);

	if (val > sMAX_DEBUG_PATH_LEN) {
		val = sMAX_DEBUG_PATH_LEN;
	}

	float f = (float)val / (float)sMAX_DEBUG_PATH_LEN;
	int icon = m_render->get(x, y)->tile.icon;

	m_renderer->setChar(x, y, icon);
	m_renderer->setCharForeground(x, y, TCODColor::lighterGrey);
	m_renderer->setCharBackground(x, y, TCODColor::lerp(TCODColor::red, TCODColor::black, f));
#endif
}

void RenderThread::done(bool d)
{
	lock();
	m_done = d;
	unlock();
}

bool RenderThread::isDone()
{
	bool d = false;

	lock();
	d = m_done;
	unlock();

	return d;
}
