#pragma once

#include "libtcod/libtcod.hpp"
#include <vector>
#include <set>

#include "util.h"
#include "geometry.h"
#include "common.h"
#include "fov/fov.h"

class Light
{
public:
	Light(int x, int y, float level, int rad, const Color& c = Color::white);
	~Light();

	Point position;
	Color color;

	// the level of the light source
	float lightLevel;

	// the radius of the light (the effective light
	// level at this spot will be lightLevel * (dr/radius)
	int radius;

	struct Ray
	{
		bool enabled;
		float angle;
		fov_direction_type direction;
	};

	Ray ray;

	void calculateLighting(TheGrid* grid);

protected:
	fov_settings_type m_fov_settings;
};


// The engine which calculates lighting in game
class LightingEngine : public Utils::Singleton<LightingEngine>
{
public:
	LightingEngine() {}

	static const int sMAX_LIGHT_LEVEL;

	static void apply_visible(void *map, int x, int y, int dx, int dy, void *src);
	static void apply_light(void *map, int x, int y, int dx, int dy, void *src);

	static int opaque(void *map, int x, int y);

	void calculateLighting(TheGrid* grid, const Rect& renderRect);

	void addLight(Light *l);
	void removeLight(Light *l);

protected:

	std::set<Light*> m_lights;
};
