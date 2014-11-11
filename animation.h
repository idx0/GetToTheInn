#pragma once

#include "common.h"
#include "lighting.h"
#include "object.h"
#include "delay.h"
#include "util.h"

#include <list>

class Frame
{
public:
	Frame() {}
	virtual ~Frame() {}

	// returns the icon for this frame
	int icon;

	// returns the color for this frame
	TCODColor color;

	// the lighting model for this frame
	LightingModel lightingModel; 

	// the mobility model for this frame
	MobilityModel mobilityModel;
};


class Sprite : public Object
{
public:
	Sprite(const Point& pos, unsigned int repeat);
	Sprite(int x, int y, unsigned int repeat);
	virtual ~Sprite();

	virtual void update();
	virtual void interact();

	void addFrame(Frame* frm, unsigned int length);

	static const int FOREVER;

public:

	virtual Tile tile() const;

	virtual LightingModel lightingModel() const;
	virtual MobilityModel mobilityModel() const;

protected:

	struct SpriteFrame
	{
		int		frameLen;
		Frame*	frame;
	};

	typedef std::list<SpriteFrame> FrameList;

	FrameList m_frames;
	FrameList::iterator m_currentFrame;

	unsigned int m_frameTicks;

	// repeat
	unsigned int m_repeat;
	unsigned int m_plays;
};


///////////////////////////////////////////////////////////////////////////////

class PhantomWall : public Sprite
{
public:
	PhantomWall(int x, int y);
	~PhantomWall();

	virtual void interact();

protected:

	bool m_interacted;
};


///////////////////////////////////////////////////////////////////////////////

class ADSR
{
public:
	enum Type
	{
		ATTACK = 0,
		DECAY,
		SUSTAIN,
		RELEASE
	};

	typedef float (*transform_func)(float);

	static float t_linear(float f);
	static float t_square(float f);
	static float t_log(float f);
	static float t_sqrt(float f);

public:
	
	ADSR(float amp, transform_func f);	// ADSR with simply attack only function
	ADSR(float amp = 1.0f,
		 float da = 0.0f, float dd = 0.0f, float ds = 1.0f, float dr = 0.0f,
		 transform_func ta = NULL, transform_func td = NULL, transform_func tr = NULL);

	float transform(float r) const;

	ADSR& operator=(const ADSR& rhs);

	void initialize(float amp, float da, float dd, float ds, float dr, 
					transform_func ta, transform_func td, transform_func tr);

protected:

	// proportionally scales down or up the durations so their sum equals unity
	void normalize();

protected:
	float m_amplitude;	// sustain amplitude
	float m_duration[4];

	transform_func m_attackFunc;
	transform_func m_decayFunc;
	transform_func m_releaseFunc;
};

// # id, lifetime,	repeat, delay,	adsr (can also be inlined)
// { id, value,		value,	value,	id }
// exp: perlin delay:
//		MULT(10.0,PRLN(RNDN()))
class Animation
{
public:

	static const int FOREVER = 0;

public:
	Animation(int lifetime = FOREVER, int repeat = FOREVER);
	virtual ~Animation();

	virtual void tick() = 0;
	virtual bool done() const;

protected:

	bool m_done;
	
	int m_lifetime;
	int m_age;

	int m_repeat;

	ADSR m_adsr;
	Delay* m_delay;
};

// # id, func, base,  to,    animation (can also be inlined)
// { id, enum, color, color, id }
class ColorAnimation : public Animation
{
public:

	// uses ADSR envelope
	struct Operation
	{
		enum Type
		{
			A_COLOR_CONST,
			A_COLOR_LERP,	// lerps color from 'base' to 'to' via function
			A_COLOR_FADE,	// fades a color based on function
			A_COLOR_GREY,	// greys a color based on function
		};

		Type type;
		Color base;
		Color to;
	};

	virtual void tick();

	Color color() const;

protected:

	void set();

protected:

	Color m_color;
	Operation m_operation;
};

///////////////////////////////////////////////////////////////////////////////

class Animator : public Utils::Singleton<Animator>
{
public:
	struct Cell
	{
		LightingModel	lighting;
		bool			needsRender;
		Color			fg;
		Color			bg;
		int				icon;
	};

protected:

};
