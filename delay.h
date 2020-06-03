#pragma once

// The delay class is used to define a delay model for timing animations and
// particle effects.  The delay class can be ticked each update, then calling
// delay() will return true if the process should block for the remainder of
// this update, or false if the process should continue
class Delay
{
public:
	Delay();
	virtual ~Delay();

	// returns true if delay triggered
	bool tick();

	// return true if process should delay
	bool delay() const { return !m_ready; }

	// resets count to 0.  This function can be
	// overwritten to set m_delay as well to give
	// variable delays
	virtual void reset() = 0;

	virtual Delay* copy() const = 0;

protected:

	bool m_ready;

	int m_delay;
	int m_count;
};

///////////////////////////////////////////////////////////////////////////////

class PerlinDelay : public Delay
{
public:
	PerlinDelay();

	Delay* copy() const;

protected:

	void reset();

	double m_x;
};

class ConstantDelay : public Delay
{
public:
	ConstantDelay(int nticks);

	Delay* copy() const;

    void restart(int nticks);

    inline int timeout() const { return m_delay; }
    inline int count() const { return m_count; }
    float percent() const;
	void reset();
};

