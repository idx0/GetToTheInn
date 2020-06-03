#pragma once

#include "raylib.h"

#include "fov/fov.h"
#include "common.h"
#include "lighting.h"
#include "object.h"


#include "effects.h"

//#define TORCH_FLICKER

class Player : public Object
{
public:

	static void apply_visible(void *map, int x, int y, int dx, int dy, void *src);

public:
	Player(int x, int y);
	~Player();

	void update(TheGrid* grid);

	// returns true if the player actually can move (and does)
	bool move(int dx, int dy, const TheGrid* grid);

    Point inFrontOf() const;

	// player attributes (TODO)
	int sight;
	int hearing;
	int light;

	fov_direction_type direction;

protected:

#ifdef TORCH_FLICKER
	PerlinDelay m_delay;
#endif

	fov_settings_type m_visionFOV;
};
