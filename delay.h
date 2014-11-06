#pragma once

// The delay class is used to define a delay model for timing animations and
// particle effects.  The delay class can be ticked each update, then calling
// delay() will return true if the process should block for the remainder of
// this update, or false if the process should continue
class Delay
{
public:

	// returns true if delay triggered
	virtual bool tick() = 0;

	// return true if process should delay
	virtual bool delay() const {
		return (m_delay != m_count);
	}

protected:
	int m_delay;
	int m_count;
};

///////////////////////////////////////////////////////////////////////////////

class PerlinDelay : public Delay
{
public:
	PerlinDelay();

	bool tick();
	bool delay() const;

protected:

	void reset();

	double m_x;
	bool m_ready;
};

class ConstantDelay : public Delay
{
public:
	ConstantDelay(int nticks);

	bool tick();
	bool delay() const;
protected:

	void reset();

	bool m_ready;
};

