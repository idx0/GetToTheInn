#include <algorithm>
#include <math.h>

#include "effects.h"
#include "rnd.h"


///////////////////////////////////////////////////////////////////////////////

Particle::Particle(const Point& pos) :
	m_color(TCODColor::black), m_icon(0),
	m_location((float)pos.x(), (float)pos.y()),
	m_velocity(0.0f, 0.0f), m_force(0.0f, 0.0f),
	m_dead(false)
{
}

void Particle::addForce(const PointF& force)
{
	m_force += force;
}

void Particle::update()
{
	// we're not violating the laws of physics here - we're picking easy
	// constants - mass = 1, delta time = 1
	m_velocity += m_force;
	m_location += m_velocity;

	m_force = PointF(0.0f, 0.0f);
}


bool Particle::dead() const
{
	return m_dead;
}

///////////////////////////////////////////////////////////////////////////////

Emitter::Emitter() : m_delay(NULL), m_repeat(0), m_count(0)
{
}

PointEmitter::PointEmitter(const Point& o) : m_origin(o)
{
	m_repeat = 10;
	m_count = 0;

	m_delay = new ConstantDelay(1);
}

PointEmitter::~PointEmitter()
{
	delete m_delay;
}

void PointEmitter::update()
{
	m_delay->tick();

	// emit
	emit();

	// then update
	ParticleList::iterator it = m_particles.begin();

	while (it != m_particles.end()) {
		(*it)->update();

		if ((*it)->dead()) {
			it = m_particles.erase(it);
		} else {
			it++;
		}
	}
}

void PointEmitter::emit()
{
	static const float S2PI = 6.28318530718f;
	static const float dtheta = (S2PI / 32.0f);
	float theta = 0.0f;

	static const float iv = 0.6f;

	if (((!m_delay->delay()) && (m_count < m_repeat)) || (m_count == 0)) {
		while (theta < S2PI) {
			PointF f((iv * cos(theta)), (iv * sin(theta)));

			FlameParticle* p = new FlameParticle(m_origin);
			p->addForce(Rnd::rndn() * f);

			m_particles.push_back(p);

			theta += dtheta * (1 + Rnd::rndg());
		}

		m_count++;
	}
}

void PointEmitter::render(TCODConsole* screen)
{
	ParticleList::iterator it = m_particles.begin();

	while (it != m_particles.end()) {
		PointF p = (*it)->position();

		Tile t = (*it)->tile();

		if (t.icon == 0) {
			screen->setCharBackground((int)p.x(), (int)p.y(), t.fgColor.toTCOD());
		} else {
			screen->setChar((int)p.x(), (int)p.y(), t.icon);
			screen->setCharForeground((int)p.x(), (int)p.y(), t.fgColor.toTCOD());
		}
		it++;
	}
}

AmbientEmitter::AmbientEmitter(const Size& size) : m_size(size), m_eight(false)
{
	int i;
	m_map = new Particle*[size.area()];

	for (i = 0; i < size.area(); i++) { m_map[i] = NULL; }

	// seed emitters
	i = 0;
	static const int SEEDS = 20;
	while (i < SEEDS) {
		int x = Rnd::between(0, size.width());
		int y = Rnd::between(0, size.height());
		int j = x + size.width() * y;

		if (m_map[j] == NULL) {
			m_map[j] = new FlameParticle(Point(x, y));
			m_particles.push_back(m_map[j]);
			i++;
		}
	}
}

AmbientEmitter::~AmbientEmitter()
{
	delete[] m_map;
}

void AmbientEmitter::update()
{
	// emit
	emit();

	// then update
	ParticleList::iterator it = m_particles.begin();

	while (it != m_particles.end()) {
		(*it)->update();

		if ((*it)->dead()) {
			it = m_particles.erase(it);
		} else {
			it++;
		}
	}
}

void AmbientEmitter::render(TCODConsole* screen)
{
	ParticleList::iterator it = m_particles.begin();

	while (it != m_particles.end()) {
		PointF p = (*it)->position();

		Tile t = (*it)->tile();

		if (t.icon == 0) {
			screen->setCharBackground((int)p.x(), (int)p.y(), t.fgColor.toTCOD());
		} else {
			screen->setChar((int)p.x(), (int)p.y(), t.icon);
			screen->setCharForeground((int)p.x(), (int)p.y(), t.fgColor.toTCOD());
		}
		it++;
	}
}

void AmbientEmitter::emit()
{
	int dir, x, y;

	ParticleList::iterator it = m_particles.begin();

	for (; it != m_particles.end(); it++) {
		int ddir = NNEIGHBORS;
		const DN* dirs;

		if (m_eight) {
			dirs = NEIGHBORS;
		} else {
			ddir = NCARDINAL;
			dirs = CARDINAL;
		}

		// place
		dir = Rnd::between(0, ddir);
		for (int i = 0; i < ddir; i++) {
			dir = (dir + i) % ddir;
			x = (*it)->position().x() + dirs[dir].dx;
			y = (*it)->position().y() + dirs[dir].dy;
			int j = x + m_size.width() * y;

			if (m_map[j] == NULL) {
				m_map[j] = new FlameParticle(Point(x, y));
				m_particles.push_back(m_map[j]);
				break;
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////////////

LinearParticle::LinearParticle(const Point& pos, int lifetime) :
	Particle(pos),
	m_lifetime(lifetime)
{
}

void LinearParticle::update()
{
	Particle::update();

	if (m_lifetime == 0) {
		m_dead = true;
	} else {
		m_lifetime--;
	}
}

///////////////////////////////////////////////////////////////////////////////

FlameParticle::FlameParticle(const Point& pos) :
	LinearParticle(pos, FPS),
	m_animation(Gradient(4, Color(61, 15, 0).packed(),
							Color::flame.packed(),
							Color::orange.packed(),
							Color::yellow.packed()), FPS),
	m_extent(8),
	m_totalLife(FPS)
{
//	int fire_idx[] = { 0, 3, 7, 11 };
//	TCODColor fire_col[] = {
//		TCODColor::flame,
//		TCODColor::orange,
//		TCODColor::yellow,
//		TCODColor::white
//	};
//
//	TCODColor::genMap(m_fire, 4, fire_col, fire_idx);

	m_animation.setDelay(new ConstantDelay(1));
}

void FlameParticle::update()
{
	static const float burst = 0.25f;
	float life = 1.0f - ((float)m_lifetime / (float) m_totalLife);
	float var = burst * (1.0f - 2.0f * Rnd::rndn());
	
	if (m_extent) {
		m_color = TCODColor::lerp(TCODColor::yellow, TCODColor::lighterYellow, std::min(1.0f, life / burst + var));
		m_extent--;

		Particle::update();
	} else {
		m_color = TCODColor::lerp(TCODColor::yellow, TCODColor::darkestFlame, std::min(1.0f, life + var));
	}

	if (m_lifetime == 0) {
		m_dead = true;
	} else {
		m_lifetime--;
	}
}


const int AshParticle::sCHAR = 249;

AshParticle::AshParticle(const Point& o) :
	Particle(o), m_delay(20), m_lifetime(20),
	m_lifespan(20), m_pos(o)
{
	m_icon = sCHAR;
}

void AshParticle::update()
{
	if (m_lifetime == 0) {
		m_dead = true;
	} else if (m_delay.tick()) {
		// brownian motion
		float rx = Rnd::rndg(), ry = Rnd::rndg();

		m_pos += PointF(rx, ry);

		m_location = m_pos;
		m_color = TCODColor::lerp(TCODColor::darkGrey, TCODColor::lightOrange,
								  (float)m_lifetime / (float)m_lifespan);

		m_lifetime--;
	}
}
