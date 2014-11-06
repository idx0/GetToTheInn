#include "delay.h"
#include "rnd.h"

#include <math.h>


PerlinDelay::PerlinDelay() : m_x(0), m_ready(false)
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

bool PerlinDelay::tick()
{
	m_ready = false;
	m_count++;

	if (m_count >= m_delay) {
		reset();

		m_ready = true;
	}

	return m_ready;
}

bool PerlinDelay::delay() const
{
	return !m_ready;
}

///////////////////////////////////////////////////////////////////////////////

ConstantDelay::ConstantDelay(int nticks) : m_ready(false)
{
	m_delay = nticks;
	reset();
}

void ConstantDelay::reset()
{
	m_count = 0;
}

bool ConstantDelay::tick()
{
	m_ready = false;
	m_count++;

	if (m_count >= m_delay) {
		reset();

		m_ready = true;
	}

	return m_ready;
}

bool ConstantDelay::delay() const
{
	return !m_ready;
}
