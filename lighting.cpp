#include "lighting.h"

#include "sys/logger.h"

const int LightingEngine::sMAX_LIGHT_LEVEL = 10;

///////////////////////////////////////////////////////////////////////////////

void LightingEngine::apply_light(void *map, int x, int y, int dx, int dy, void *src)
{
	TheGrid* g = (TheGrid *)map;
	Light *l = (Light *)src;

	if ((g) && (l) && (g->inbounds(x, y))) {
		// the lighting calculate is based on a quadratic equation.  It provides
		// a nice look, while giving a nonlinear fall-off.
		// It is based on a simple formula
		//		L1 = L0 * (1 - dr^2 / rad^2)
		// Here, the resulting light level, L1, is given as a fraction of the original
		// light level, L0.  dr is the distance from the light source, equal to
		//		sqrt(dx^2 + dy^2); dx, dy from inputs
		// And rad is the radius of the light (literally l->radius).
		float dd = FSQR(l->radius);
		float dr = std::min(dd, FSQR(dx) + FSQR(dy));
		float coef = (1 - (dr / dd));

		g->at(x, y)->render->lighting.lightColor += l->color * coef * l->lightLevel;

		if (coef > 0) {
			g->at(x, y)->render->lighting.flags |= L_LIT;
		}
	}
}

int LightingEngine::opaque(void *map, int x, int y)
{
	TheGrid* g = (TheGrid *)map;

	if ((g) && (g->inbounds(x, y))) {
		return ((g->get(x, y)->render->lighting.flags & L_TRANSPARENT) ? FOV_FALSE : FOV_TRUE);
	}

	return FOV_TRUE;
}

///////////////////////////////////////////////////////////////////////////////

Light::Light(int x, int y, float level, int rad, const Color& c) :
	position(Point(x, y)),
	color(c),
	lightLevel(level),
	radius(rad)
{
	ray.angle = 0.0f;
	ray.direction = FOV_NORTH;
	ray.enabled = false;

	fov_settings_init(&m_fov_settings);
	fov_settings_set_opacity_test_function(&m_fov_settings, LightingEngine::opaque);
    fov_settings_set_apply_lighting_function(&m_fov_settings, LightingEngine::apply_light);

	LightingEngine::getInstance()->addLight(this);
}

Light::~Light()
{
	LightingEngine::getInstance()->removeLight(this);
	fov_settings_free(&m_fov_settings);
}

void Light::calculateLighting(TheGrid* grid)
{
	if (ray.enabled) {
		fov_beam(&m_fov_settings,
				 grid, this,
				 position.x(),
				 position.y(),
				 radius,
				 ray.direction,
				 ray.angle);
	} else {
		fov_circle(&m_fov_settings,
				   grid, this,
				   position.x(),
				   position.y(),
				   radius);
	}
}

///////////////////////////////////////////////////////////////////////////////

void LightingEngine::addLight(Light *l)
{
	m_lights.insert(l);
}

void LightingEngine::removeLight(Light *l)
{
	m_lights.erase(l);
}

void LightingEngine::calculateLighting(TheGrid* grid, const Rect& renderRect)
{
	std::set<Light*>::iterator it = m_lights.begin();

	while (it != m_lights.end()) {
		if (renderRect.inbounds((*it)->position)) {
			(*it)->calculateLighting(grid);
		}

		it++;
	}
}
