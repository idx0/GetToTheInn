#pragma once

#include "util.h"
#include "geometry.h"
#include "common.h"
#include "animation.h"

#include <list>
#include <libtcod.hpp>


///////////////////////////////////////////////////////////////////////////////

class Particle
{
public:
	Particle(const Point& pos);
	virtual ~Particle() {}

	void addForce(const PointF& force);

	virtual void update();
	bool dead() const;	

	PointF position() const;

	Tile tile() const;

protected:
	TCODColor m_color;
	int m_icon;

	PointF m_location;

	PointF m_velocity;
	PointF m_force;	// acceleration added once during update
	
	bool m_dead;
};

inline
PointF Particle::position() const
{
	return m_location;
}

inline
Tile Particle::tile() const
{
	return Tile(m_color, m_icon);
}

///////////////////////////////////////////////////////////////////////////////

// the emitter class for emitting particles
class Emitter
{
public:
	enum Rate {
		CONSTANT,
		SINE
	};

public:

	Emitter();

	virtual void update() = 0;
	virtual void render(TCODConsole* screen) = 0;

protected:

	typedef std::list<Particle*> ParticleList;

	ParticleList m_particles;

	virtual void emit() = 0;

	Delay* m_delay;

	int m_repeat;
	int m_count;
};

class PointEmitter : public Emitter
{
public:

	PointEmitter(const Point& o);
	~PointEmitter();

	virtual void update();
	virtual void render(TCODConsole* screen);

protected:
	virtual void emit();

	Point m_origin;
};

class AmbientEmitter : public Emitter
{
public:
	AmbientEmitter(const Size& size);
	~AmbientEmitter();

	virtual void update();
	virtual void render(TCODConsole* screen);

protected:

	Size m_size;
	Particle** m_map;
	bool m_eight;

	virtual void emit();
};

// TODO:
//  effects can directly modify the LightingModel to add light??
//  timing/animation?
//  non-uniform particle lifetime/shared or global particle lifetime
//  emitters emitting emitters
//
//  http://codegolf.stackexchange.com/questions/24462/display-the-explosion-of-a-star-in-ascii-art
//  http://www.reddit.com/r/gamedev/comments/246wa9/roguelike_ascii_animations/

///////////////////////////////////////////////////////////////////////////////

class LinearParticle : public Particle
{
public:
	LinearParticle(const Point& pos, int lifetime);

	virtual void update();

protected:

	int m_lifetime;
};

class FlameParticle : public LinearParticle
{
public:
	FlameParticle(const Point& pos);

	virtual void update();

protected:

//	TCODColor m_fire[12];
	ColorAnimation m_animation;

	int m_extent;
	int m_totalLife;
};

class AshParticle : public Particle
{
public:
	AshParticle(const Point& o);

	virtual void update();

	static const int sCHAR;

protected:

	ConstantDelay m_delay;

	int m_lifetime;
	int m_lifespan;

	PointF m_pos;
};
