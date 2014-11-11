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
	frm->lightingModel.ambientColor = Color(96, 96, 96);

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
		frm->lightingModel.ambientColor = Color(96, 96, 96);

		addFrame(frm, 20);

		frm = new Frame;
		frm->color = TCODColor::lerp(c, TCODColor::white, 0.40f);
		frm->icon = 'x';
		frm->lightingModel.ambientColor = Color(96, 96, 96);

		addFrame(frm, 20);

		frm = new Frame;
		frm->color = TCODColor::lerp(c, TCODColor::white, 0.60f);
		frm->icon = 'O';
		frm->lightingModel.flags |= L_TRANSPARENT;
		frm->lightingModel.ambientColor = Color(96, 96, 96);

		addFrame(frm, 20);

		frm = new Frame;
		frm->color = TCODColor::lerp(c, TCODColor::white, 0.80f);
		frm->icon = 'o';
		frm->lightingModel.flags |= L_TRANSPARENT;
		frm->lightingModel.ambientColor = Color(96, 96, 96);

		addFrame(frm, 20);

		frm = new Frame;
		frm->color = TCODColor::lerp(c, TCODColor::white, 1.0f);
		frm->icon = '.';
		frm->lightingModel.flags |= L_TRANSPARENT;
		frm->lightingModel.ambientColor = Color(96, 96, 96);
		frm->mobilityModel.flags |= (M_WALKABLE | M_JUMPABLE | M_REACHABLE);

		addFrame(frm, 20);

		m_repeat = 1;
		m_plays	= 1;
		m_currentFrame = m_frames.begin();
	}
}

///////////////////////////////////////////////////////////////////////////////

ADSR::ADSR(float amp, transform_func f)
{
	initialize(amp, 1.0f, 0.0f, 0.0f, 0.0f, f, NULL, NULL);
}

ADSR::ADSR(float amp, float da, float dd, float ds, float dr,
		   transform_func ta, transform_func td, transform_func tr)
{
	initialize(amp, da, dd, ds, dr, ta, td, tr);
}

float ADSR::t_linear(float f)
{
	return f;
}

float ADSR::t_square(float f)
{
	return FSQR(f);
}

float ADSR::t_log(float f)
{
	return log(1 + 9 * f);
}

float ADSR::t_sqrt(float f)
{
	return sqrt(f);
}

ADSR& ADSR::operator=(const ADSR& rhs)
{
	if (this != &rhs) {
		m_amplitude		= rhs.m_amplitude;

		m_duration[ATTACK]	= rhs.m_duration[ATTACK];
		m_duration[DECAY]	= rhs.m_duration[DECAY];
		m_duration[SUSTAIN] = rhs.m_duration[SUSTAIN];
		m_duration[RELEASE] = rhs.m_duration[RELEASE];

		m_attackFunc	= rhs.m_attackFunc;
		m_decayFunc		= rhs.m_decayFunc;
		m_releaseFunc	= rhs.m_releaseFunc;
	}

	return *this;
}

float ADSR::transform(float r) const
{
	if (r < m_duration[0]) {
		float ar = r / m_duration[0];
		return m_attackFunc(ar);
	} else if (r < m_duration[1]) {
		float dr = (r - m_duration[0]) / m_duration[1];
		float da = 1.0f - m_amplitude;
		return m_amplitude + da * (1.0f - m_decayFunc(dr));
	} else if (r < m_duration[2]) {
		return m_amplitude;
	} else {
		float rr = (r - (1.0f - m_duration[3])) / m_duration[3];
		return m_amplitude * (1 - m_releaseFunc(r));
	}
}

void ADSR::normalize()
{
	float sum = m_duration[ATTACK] + m_duration[DECAY] +
				m_duration[SUSTAIN] + m_duration[RELEASE];

	m_duration[ATTACK]	/= sum;
	m_duration[DECAY]	/= sum;
	m_duration[SUSTAIN] /= sum;
	m_duration[RELEASE] /= sum;
};

void ADSR::initialize(
	float amp, float da, float dd, float ds, float dr, 
	transform_func ta, transform_func td, transform_func tr)
{
	m_amplitude = std::min(1.0f, std::max(0.0f, amp));

	float dsum = da + dd + ds + dr;

	if ((dsum <= 0) ||
		(da < 0) || (dd < 0) || (ds < 0) || (dr < 0)) {
		m_duration[ATTACK]	= 0.0f;
		m_duration[DECAY]	= 0.0f;
		m_duration[SUSTAIN] = 1.0f;
		m_duration[RELEASE] = 0.0f;
	} else {
		m_duration[ATTACK]	= da;
		m_duration[DECAY]	= dd;
		m_duration[SUSTAIN] = ds;
		m_duration[RELEASE] = dr;
	}

	// set attack
	if ((m_duration[ATTACK] > 0.0f) && (ta = NULL)) {
		m_duration[ATTACK] = 0.0f;
	}
	m_attackFunc = ta;

	// set decay
	if ((m_duration[DECAY] > 0.0f) && (td = NULL)) {
		m_duration[DECAY] = 0.0f;
	}
	m_decayFunc = td;

	// must have attack to have decay
	if (m_duration[ATTACK] == 0.0f) {
		m_duration[DECAY] = 0.0f;
		m_decayFunc = NULL;
	}

	// set release
	if ((m_duration[RELEASE] > 0.0f) && (tr = NULL)) {
		m_duration[RELEASE] = 0.0f;
	}
	m_releaseFunc = tr;

	normalize();
}

///////////////////////////////////////////////////////////////////////////////

Animation::Animation() : m_done(false)
{
}

Animation::~Animation()
{
}

bool Animation::done() const
{
	return m_done;
}

void ColorAnimation::tick()
{
}

void ColorAnimation::set()
{
	float r = (float)m_age / (float)m_lifetime;

	switch (m_operation.type) {
	default:
	case Operation::A_COLOR_CONST: break;
	case Operation::A_COLOR_LERP:
		break;
	case Operation::A_COLOR_FADE:
		break;
	case Operation::A_COLOR_GREY:
		break;
	}
}

Color ColorAnimation::color() const
{
}