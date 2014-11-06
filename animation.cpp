#include "animation.h"

const int Sprite::FOREVER = 0;

Sprite::Sprite(const Point& pos, unsigned int repeat) :
	Object(pos, ' ', TCODColor::black),
	m_frameTicks(0),
	m_repeat(repeat),
	m_plays(repeat)
{
}

Sprite::Sprite(int x, int y, unsigned int repeat) :
	Object(x, y, ' ', TCODColor::black),
	m_frameTicks(0),
	m_repeat(repeat),
	m_plays(repeat)
{
	m_currentFrame = m_frames.end();
}

Sprite::~Sprite()
{
	// TODO: delete frames?
}

void Sprite::update()
{
	FrameList::iterator next;

	if ((m_plays > 0) || (m_repeat == FOREVER)) {
		m_frameTicks++;
		next = m_currentFrame;

		if ((m_currentFrame != m_frames.end()) &&
			(m_frameTicks >= m_currentFrame->frameLen)) {
			next++;

			// show next frame
			if (next != m_frames.end()) {
				m_currentFrame++;
			} else {
				// repeat
				if (m_plays > 1) {
					m_currentFrame = m_frames.begin();
				}
				m_plays--;
			}

			m_frameTicks = 0;
		}
	}
}

void Sprite::interact()
{
	Object::interact();
}

Tile Sprite::tile() const
{
	if ((m_currentFrame != m_frames.end()) &&
		(Utils::isValid(m_currentFrame->frame))) {
		return Tile(m_currentFrame->frame->color,
					m_currentFrame->frame->icon);
	}
	return Tile();
}

LightingModel Sprite::lightingModel() const
{
	if ((m_currentFrame != m_frames.end()) &&
		(Utils::isValid(m_currentFrame->frame))) {
		return m_currentFrame->frame->lightingModel;
	}
	return LightingModel();
}

MobilityModel Sprite::mobilityModel() const
{
	if ((m_currentFrame != m_frames.end()) &&
		(Utils::isValid(m_currentFrame->frame))) {
		return m_currentFrame->frame->mobilityModel;
	}
	return MobilityModel();
}

void Sprite::addFrame(Frame* frm, unsigned int length)
{
	SpriteFrame s;
	s.frame = frm;
	s.frameLen = length;

	m_frames.push_back(s);

	if (m_currentFrame == m_frames.end()) {
		m_currentFrame = m_frames.begin();
	}
}

///////////////////////////////////////////////////////////////////////////////

PhantomWall::PhantomWall(int x, int y) :
	Sprite(x, y, FOREVER),
	m_interacted(false)
{
	Frame* frm = new Frame;

	frm->color = TCODColor::lerp(TCODColor::grey, TCODColor::azure, 0.20f);
	frm->icon = '#';
	frm->lightingModel.transparent = false;
	frm->lightingModel.ambientLevel = 3.5f;
	frm->mobilityModel.walkable = false;
	frm->mobilityModel.jumpable = false;
	frm->mobilityModel.reachable = false;

	addFrame(frm, 20);
}

PhantomWall::~PhantomWall()
{
	FrameList::iterator it = m_frames.begin();

	for (; it != m_frames.end(); it++) {
		delete (*it).frame;
	}
}

void PhantomWall::interact()
{
	if (!m_interacted) {
		m_interacted = true;

		Frame* frm = new Frame;
		const TCODColor c = TCODColor::lerp(TCODColor::grey, TCODColor::azure, 0.20f);

		frm->color = TCODColor::lerp(c, TCODColor::white, 0.20f);
		frm->icon = 'X';
		frm->lightingModel.transparent = false;
		frm->lightingModel.ambientLevel = 3.5f;
		frm->mobilityModel.walkable = false;
		frm->mobilityModel.jumpable = false;
		frm->mobilityModel.reachable = false;

		addFrame(frm, 20);

		frm = new Frame;
		frm->color = TCODColor::lerp(c, TCODColor::white, 0.40f);
		frm->icon = 'x';
		frm->lightingModel.transparent = false;
		frm->lightingModel.ambientLevel = 3.5f;
		frm->mobilityModel.walkable = false;
		frm->mobilityModel.jumpable = false;
		frm->mobilityModel.reachable = false;

		addFrame(frm, 20);

		frm = new Frame;
		frm->color = TCODColor::lerp(c, TCODColor::white, 0.60f);
		frm->icon = 'O';
		frm->lightingModel.transparent = true;
		frm->lightingModel.ambientLevel = 3.5f;
		frm->mobilityModel.walkable = false;
		frm->mobilityModel.jumpable = false;
		frm->mobilityModel.reachable = false;

		addFrame(frm, 20);

		frm = new Frame;
		frm->color = TCODColor::lerp(c, TCODColor::white, 0.80f);
		frm->icon = 'o';
		frm->lightingModel.transparent = true;
		frm->lightingModel.ambientLevel = 3.5f;
		frm->mobilityModel.walkable = false;
		frm->mobilityModel.jumpable = false;
		frm->mobilityModel.reachable = false;

		addFrame(frm, 20);

		frm = new Frame;
		frm->color = TCODColor::lerp(c, TCODColor::white, 1.0f);
		frm->icon = '.';
		frm->lightingModel.transparent = true;
		frm->lightingModel.ambientLevel = 3.5f;
		frm->mobilityModel.walkable = true;
		frm->mobilityModel.jumpable = true;
		frm->mobilityModel.reachable = true;

		addFrame(frm, 20);

		m_repeat = 1;
		m_plays	= 1;
		m_currentFrame = m_frames.begin();
	}
}
