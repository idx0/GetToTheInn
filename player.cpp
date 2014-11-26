#include "player.h"
#include "rnd.h"

void Player::apply_visible(void *map, int x, int y, int dx, int dy, void *src)
{
	TheGrid* g = (TheGrid *)map;
//	Player* p = (Player *)src;

	if ((g) && (g->inbounds(x, y))) {
		g->at(x, y)->render->discover.flags |= D_SEEN;

		if (g->get(x, y)->render->lighting.flags & L_LIT) {
			// discovered!
			g->at(x, y)->render->discover.flags |= D_EXPLORED;
		} else {
			g->at(x, y)->render->discover.flags |= D_HIDDEN;
		}
	}
}


Player::Player(int x, int y) :
	Object(x, y, '@', TCODColor::white),
	sight(28), hearing(20), light(14),
	direction(FOV_EAST)
{
	m_light = new Light(x, y, 1.15f, Rnd::between(14,18), Color(255, 255, 171));

	fov_settings_init(&m_visionFOV);
	fov_settings_set_opacity_test_function(&m_visionFOV, LightingEngine::opaque);
    fov_settings_set_apply_lighting_function(&m_visionFOV, Player::apply_visible);
}

Player::~Player()
{
	delete m_light;
}

bool Player::move(int dx, int dy, const TheGrid* grid)
{
	int x = m_position.x() + dx;
	int y = m_position.y() + dy;

	bool ii = grid->inbounds(x, y);

	if ((ii) && (grid->get(x, y)->render->mobility.flags & M_WALKABLE)) {
		m_position = Point(x, y);
		m_light->position = Point(x, y);

		if (dx == 0) {
			if (dy < 0) {
				direction = FOV_NORTH;
			} else if (dy > 0) {
				direction = FOV_SOUTH;
			}
		} else if (dy == 0) {
			if (dx < 0) {
				direction = FOV_WEST;
			} else if (dx > 0) {
				direction = FOV_EAST;
			}
		} else if (dx < 0) {
			if (dy < 0) {
				direction = FOV_NORTHWEST;
			} else if (dy > 0) {
				direction = FOV_SOUTHWEST;
			}
		} else if (dx > 0) {
			if (dy < 0) {
				direction = FOV_NORTHEAST;
			} else if (dy > 0) {
				direction = FOV_SOUTHEAST;
			}
		}

		return true;
	}

	return false;
}

void Player::update(TheGrid *grid)
{

#if 0
	if (Engine::e->beam) {
		m_playerLight.ray.enabled = true;
		m_playerLight.ray.angle = Engine::e->beam_angle;
		m_playerLight.ray.direction = Engine::e->player()->direction;
	} else {
#else
	{
#endif
		m_light->ray.enabled = false;

#ifdef TORCH_FLICKER
		m_delay.tick();

		if (!m_delay.delay()) {
			m_light->lightLevel = Rnd::between(6.5f, 10.0f);
		}
#endif
	}


	// make sure we calculate lighting before the player fov, because lighting
	// helps determine true visibility
#if 0
	if (Engine::e->beam) {
		fov_beam(&m_fov_settings, this, NULL,
				 Engine::e->player()->x,
				 Engine::e->player()->y,
				 Engine::e->player()->sight,
				 Engine::e->player()->direction,
				 Engine::e->beam_angle);
	} else {
#else
	{
#endif
		fov_circle(&m_visionFOV, grid, this, m_position.x(), m_position.y(), sight);
	}
}
