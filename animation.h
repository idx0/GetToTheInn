#pragma once

#include "common.h"
#include "lighting.h"
#include "object.h"

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
