#include "delay.h"
#include "rnd.h"

#include <math.h>

Delay::Delay() : m_ready(false), m_delay(1), m_count(0)
{
}

Delay::~Delay()
{
}

bool Delay::tick()
{
	m_ready = false;
	m_count++;

	if (m_count >= m_delay) {
		reset();

		m_ready = true;
	}

	return m_ready;
}


///////////////////////////////////////////////////////////////////////////////

PerlinDelay::PerlinDelay() : m_x(0)
{
	reset();
}

void PerlinDelay::reset()
{
	static const double freq = 10.0;

	m_x += Rnd::rndn();
	m_delay = (int)(freq * fabs(Rnd::perlin(m_x)));
	m_count = 0;
}

Delay* PerlinDelay::copy() const
{
	PerlinDelay* r = new PerlinDelay();
	r->m_x = 0;
	r->m_count = m_count;
	r->m_delay = m_delay;
	r->m_ready = m_ready;

	return r;
}

///////////////////////////////////////////////////////////////////////////////

ConstantDelay::ConstantDelay(int nticks)
{
	m_delay = nticks;
	reset();
}

void ConstantDelay::reset()
{
	m_count = 0;
    m_ready = (m_delay == 0);
}

void ConstantDelay::restart(int nticks)
{
    m_delay = nticks;
    reset();
}

float ConstantDelay::percent() const
{
    if (m_delay != 0.0f) {
        return (float)m_count / (float)m_delay;
    }

    return 1.0f;
}

Delay* ConstantDelay::copy() const
{
	ConstantDelay* r = new ConstantDelay(m_delay);
	r->m_count = m_count;
	r->m_ready = m_ready;

	return r;
}
