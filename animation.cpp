#include "animation.h"

#if 0
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
#endif

///////////////////////////////////////////////////////////////////////////////

ADSR::ADSR(float amp, func f)
{
	initialize(amp, 1.0f, 0.0f, 0.0f, 0.0f, f, NULL, NULL);
}

ADSR::ADSR(float amp, float da, float dd, float ds, float dr,
		   func ta, func td, func tr)
{
	initialize(amp, da, dd, ds, dr, ta, td, tr);
}

float ADSR::LINEAR(float f)
{
	return f;
}

float ADSR::SQUARE(float f)
{
	return FSQR(f);
}

float ADSR::LOG(float f)
{
	return log(1 + 9 * f);
}

float ADSR::SQRT(float f)
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
	func ta, func td, func tr)
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

Animation::Animation(int lifetime, int repeat) :
	m_done(false), m_lifetime(std::min(1, lifetime)), m_age(0),
	m_repeat(repeat), m_plays(0), m_delay(NULL)
{
}

Animation::~Animation()
{
	delete m_delay;
}

void Animation::tick()
{
	if ((!m_done) && (m_delay) && (m_delay->tick())) {
		onTick();

		m_age++;
		if (m_age >= m_lifetime) {
			m_age = 0;
			m_plays++;
			if ((m_plays > m_repeat) &&
				(m_repeat != FOREVER)) {
				m_done = true;
			}
		}
	}
}

bool Animation::done() const
{
	return m_done;
}

void Animation::setDelay(Delay* delay)
{
	delete m_delay;
	m_delay = delay;
}

void Animation::setADSR(const ADSR& adsr)
{
	m_adsr = adsr;
}

///////////////////////////////////////////////////////////////////////////////

ColorAnimation::ColorAnimation(const Color& base, int lifetime, int repeat) :
	Animation(lifetime, repeat),
	m_color(base), m_type(A_COLOR_CONST), m_base(base)
{
}

ColorAnimation::ColorAnimation(const Type& func, const Color& base, const Color& to,
							   int lifetime, int repeat) :
	Animation(lifetime, repeat),
	m_color(base), m_type(func), m_base(base), m_to(to)
{
	if (func == A_COLOR_GREY) {
		m_to = base;
		m_to.greyscale();
	}
}

ColorAnimation::ColorAnimation(const Type& func, const Color& base,
							   int lifetime, int repeat) :
	Animation(lifetime, repeat),
	m_color(base), m_type(func), m_base(base)
{
	if (func == A_COLOR_GREY) {
		m_to = base;
		m_to.greyscale();
	}
}

ColorAnimation::ColorAnimation(const Gradient& g, int lifetime, int repeat) :
	Animation(lifetime, repeat),
	m_gradient(g), m_type(A_COLOR_GRADIENT)
{
}

ColorAnimation::~ColorAnimation()
{
}

void ColorAnimation::onTick()
{
	float r = m_adsr.transform((float)m_age / (float)m_lifetime);

	switch (m_type) {
	default:
	case A_COLOR_CONST: break;
	case A_COLOR_GRADIENT:
		m_color = m_gradient.getColor(r);
		break;
	case A_COLOR_LERP:
		m_color = Color::lerp(m_base, m_to, r);
		break;
	case A_COLOR_FADE:
		m_color = m_base * r;
		break;
	case A_COLOR_GREY:
		m_color = Color::lerp(m_base, m_to, r);
		break;
	}
}

Color ColorAnimation::color() const
{
	return m_color;
}
