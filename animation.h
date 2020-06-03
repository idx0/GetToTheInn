#pragma once

#include "common.h"
#include "delay.h"
#include "util.h"

#include <list>

#if 0
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

#endif


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

	typedef float (*func)(float);

	static float LINEAR(float f);
	static float SQUARE(float f);
	static float LOG(float f);
	static float SQRT(float f);

public:
	
	ADSR(float amp, func f);	// ADSR with simply attack only function
	ADSR(float amp = 1.0f,
		 float da = 0.0f, float dd = 0.0f, float ds = 1.0f, float dr = 0.0f,
		 func ta = NULL, func td = NULL, func tr = NULL);

	float transform(float r) const;

	ADSR& operator=(const ADSR& rhs);

	void initialize(float amp, float da, float dd, float ds, float dr, 
					func ta, func td, func tr);

protected:

	// proportionally scales down or up the durations so their sum equals unity
	void normalize();

protected:
	float m_amplitude;	// sustain amplitude
	float m_duration[4];

	func m_attackFunc;
	func m_decayFunc;
	func m_releaseFunc;
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
	Animation(int lifetime, int repeat = FOREVER);
	virtual ~Animation();

	virtual void tick();
	virtual bool done() const;

	// takes ownership of a delay
	void setDelay(Delay* delay);
	Delay* delay() const;

	void setADSR(const ADSR& adsr);
	ADSR adsr() const;

protected:

	virtual void onTick() = 0;

	bool m_done;
	
	int m_lifetime;
	int m_age;

	int m_repeat;
	int m_plays;

	ADSR m_adsr;
	Delay* m_delay;
};

inline
Delay* Animation::delay() const
{
	return m_delay;
}

inline
ADSR Animation::adsr() const
{
	return m_adsr; 
}

// # id, func, base,  to,    animation (can also be inlined)
// { id, enum, color, color, id }
class ColorAnimation : public Animation
{
public:
	enum Type
	{
		A_COLOR_CONST,
		A_COLOR_GRADIENT,
		A_COLOR_LERP,	// lerps color from 'base' to 'to' via function
		A_COLOR_FADE,	// fades a color based on function
		A_COLOR_GREY,	// greys a color based on function
	};

public:
	ColorAnimation(const gtti::Color& base, int lifetime, int repeat = FOREVER);
	ColorAnimation(const Type& func, const gtti::Color& base, const gtti::Color& to,
				   int lifetime, int repeat = FOREVER);
	ColorAnimation(const gtti::Gradient& g, int lifetime, int repeat = FOREVER);
	ColorAnimation(const Type& func, const gtti::Color& base,
				   int lifetime, int repeat = FOREVER);
	virtual ~ColorAnimation();

    gtti::Color color() const;

protected:

	virtual void onTick();

protected:

    gtti::Color m_color;
	gtti::Gradient m_gradient;

	Type m_type;
    gtti::Color m_base;
    gtti::Color m_to;
};

///////////////////////////////////////////////////////////////////////////////

class Animator : public Utils::Singleton<Animator>
{
public:
	struct Cell
	{
		LightingModel	lighting;
		bool			needsRender;
        gtti::Color		fg;
        gtti::Color		bg;
		int				icon;
	};

protected:

};
