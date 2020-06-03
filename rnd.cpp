#include "rnd.h"

const uint32_t mersenne_twister::_w = 32;
const uint32_t mersenne_twister::_d = 0xffffffff;

mersenne_twister::mersenne_twister() :
    _n(624), _m(397), _r(31),
    _a(0x9906b0df), _u(11),
    _s(7), _b(0x9d2c5680),
    _t(15), _c(0xefc60000),
    _l(18), _f(1812433253)
{
    initialize(0);
}

mersenne_twister::mersenne_twister(uint32_t sx) :
    _n(624), _m(397), _r(31),
    _a(0x9906b0df), _u(11),
    _s(7), _b(0x9d2c5680),
    _t(15), _c(0xefc60000),
    _l(18), _f(1812433253)
{
    initialize(sx);
}

mersenne_twister::mersenne_twister(
    uint32_t n, uint32_t m, uint32_t r, uint32_t a, uint32_t u, uint32_t s,
    uint32_t b, uint32_t t, uint32_t c, uint32_t l, uint32_t f, uint32_t sx) :
    _n(n), _m(m), _r(r), _a(a), _u(u), _s(s),
    _b(b), _t(t), _c(c), _l(l), _f(f)
{
    initialize(sx);
}

uint32_t mersenne_twister::generate()
{
    if (_index >= _n) {
        twist();
    }

    uint32_t y = _buf[_index];

    y = y ^ ((y >> _u) & _d);
    y = y ^ ((y << _s) & _b);
    y = y ^ ((y << _t) & _c);
    y = y ^ (y >> _l);

    _index++;

    return y;
}

void mersenne_twister::seed(uint32_t s)
{
    initialize(s);
}

void mersenne_twister::initialize(uint32_t sx)
{
    _index = _n;
    _buf[0] = sx;

    for (uint32_t i = 1; i < _n; i++) {
        _buf[i] = 0xffffffff & (_f * (_buf[i - 1] ^ (_buf[i - 1] >> (_w - 2))) + i);
    }
}

void mersenne_twister::twist()
{
    static const uint32_t lower_mask = (1 << _r) - 1;
    static const uint32_t upper_mask = (0xffffffff ^ lower_mask);

    for (uint32_t i = 0; i < _n; i++) {
        uint32_t x = (_buf[i] & upper_mask) + (_buf[(i + 1) % _n] & lower_mask);
        uint32_t xA = x >> 1;

        if ((x % 2) != 0) {
            xA = xA ^ _a;
        }
        _buf[i] = _buf[(i + _m) % _n] ^ xA;
    }

    _index = 0;
}
