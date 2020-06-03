#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <stdint.h>

#include "libnoise/noise.h"

#include "util.h"


class mersenne_twister
{
    static const uint32_t _w;
    static const uint32_t _d;

    uint32_t _n, _m, _r;
    uint32_t _a;
    uint32_t _u;
    uint32_t _s, _b;
    uint32_t _t, _c;
    uint32_t _l;
    uint32_t _f;

    uint32_t _index;
    uint32_t _buf[624];

public:
    mersenne_twister();
    mersenne_twister(uint32_t seed);
    mersenne_twister(uint32_t n, uint32_t m, uint32_t r, uint32_t a, uint32_t u, uint32_t s,
        uint32_t b, uint32_t t, uint32_t c, uint32_t l, uint32_t f, uint32_t seed);
    virtual ~mersenne_twister() {}

    virtual void seed(uint32_t s);
    virtual uint32_t generate();

protected:
    virtual void initialize(uint32_t seed);
    virtual void twist();
};

class Rnd : public Utils::Singleton<Rnd>
{
public:
	static void seed(uint32_t s)
	{
		srand(s);

		getInstance()->m_perlin.SetSeed(s);
		getInstance()->m_seed = s;
        getInstance()->m_twister.seed(s);

		printf("seed := %zu\n", s);
	}

	static double prng(int *seedptr)
	{
		return ((*seedptr * 7141 + 54773) % 259200) * (1.0 / 259200.0);
	}

	static double rndn()
	{
		return ((double)getInstance()->m_twister.generate() / (double)0xffffffff);
	}

	static double perlin(double x, double y = 0.0, double z = 0.0)
	{
		return getInstance()->m_perlin.GetValue(x, y, z);
	};

	static uint32_t random()
	{
		return getInstance()->m_twister.generate();
	}

	static double rndg()
	{
		static double t = 0.0;
		static double v1, v2, r;
		double ret;

		if (t == 0) {
			do {
				v1 = 2.0 * rndn() - 1.0;
				v2 = 2.0 * rndn() - 1.0;
				r = v1*v1 + v2*v2;
			} while ((r >= 1.0) || (r == 0));

			ret = v1 * sqrt((-2.0 * log(r)) / r);
		} else {
			ret = v2 * sqrt((-2.0 * log(r)) / r);
		}

		t = 1 - t;

		return ret;
	}

	static int between(int a, int b)
	{
		int diff = b - a;

		return (int)(a + diff * rndn());
	}


	static float betweenf(float a, float b)
	{
		float diff = b - a;

		return a + diff * rndn();
	}

	static int residue(double a)
	{
		if (a - (double)((int)a) > 0) { return 1; }

		return 0;
	}

	static bool one_in(int n)
	{
		bool ret = false;

		if ((n <= 1) || (between(0, n) == 0)) {
			ret = true;
		}

		return ret;
	}

	static bool n_in_m(int n, int m)
	{
		bool ret = false;

		if ((m != 0) && ((rndn()) <= ((double)n/(double)m))) {
			ret = true;
		}

		return ret;
	}

	static int ndx(int n, int x)
	{
		int i, ret = 0;

		for (i = 0; i < n; i++) {
			ret += between(1, x + 1);
		}

		return ret;
	}

	/*----------------------------------------------------------------------*/
	/*  This function returns a random real number between the specified    */
	/* inner and outer bounds.                                              */
	/*----------------------------------------------------------------------*/

	static double random_number(double inner, double outer)
	{
		double rr = rndn();
		double range = outer - inner;
		return (rr * range + inner);
	}

	/*----------------------------------------------------------------------*/
	/*   This function returns a value within a certain variation of the    */
	/*   exact value given it in 'value'.                                   */
	/*----------------------------------------------------------------------*/

	static double about(double value, double variation)
	{
		return (value + (value * random_number(-variation, variation)));
	}

	Rnd() {}
	~Rnd() {}

#if TEST
	static void test()
	{
		FILE *fp;
		static const int TEST_MAX = 32768;
		int i;

		fopen_s(&fp, "dice-1d6.dat", "w");
		for (i = 0; i < TEST_MAX; i++) { fprintf(fp, "%d\n", ndx(1, 6)); }
		fclose(fp);

		fopen_s(&fp, "dice-2d6.dat", "w");
		for (i = 0; i < TEST_MAX; i++) { fprintf(fp, "%d\n", ndx(2, 6)); }
		fclose(fp);

		fopen_s(&fp, "gaussian.dat", "w");
		for (i = 0; i < TEST_MAX; i++) { fprintf(fp, "%f\n", rndg()); }
		fclose(fp);
	}
#endif

private:

	noise::module::Perlin m_perlin;

	uint32_t m_seed;
    mersenne_twister m_twister;

};
