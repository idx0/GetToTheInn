#include "color.h"

#include <math.h>
#include <algorithm>


// monochrome
const Color Color::white		(255, 255, 255);
const Color Color::black		(  0,   0,   0);

// greys
const Color Color::grey			(127, 127, 127);
const Color Color::slate		( 47,  79,  79);

// sepiatone
const Color Color::sepia		(127, 101,  63);
const Color Color::sienna		(160,  80,  45);
const Color Color::brown		(139,  69,  19);

// misc named
const Color Color::celadon		(172, 255, 175);
const Color Color::peach		(255, 159, 127);

// metalic
const Color Color::brass		(191, 151,  96);
const Color Color::copper		(196, 136, 124);
const Color Color::gold			(229, 191,   0);
const Color Color::silver		(203, 203, 203);

// rainbow
const Color Color::red			(255,   0,   0);
const Color Color::flame		(255,  63,   0);
const Color Color::orange		(255, 127,   0);
const Color Color::amber		(255, 191,   0);
const Color Color::yellow		(255, 255,   0);
const Color Color::neon			(191, 255,   0);
const Color Color::chartreuse	(127, 255,   0);
const Color Color::lime			( 63, 255,   0);
const Color Color::green		(  0, 255,   0);
const Color Color::mint			(  0, 255,  63);
const Color Color::sea			(  0, 255, 127);
const Color Color::turquise		(  0, 255, 191);
const Color Color::cyan			(  0, 255, 255);
const Color Color::sky			(  0, 191, 255);
const Color Color::azure		(  0, 127, 255);
const Color Color::navy			(  0,  63, 255);
const Color Color::blue			(  0,   0, 255);
const Color Color::han			( 63,   0, 255);
const Color Color::violet		(127,   0, 255);
const Color Color::purple		(191,   0, 255);
const Color Color::fuchsia		(255,   0, 255);
const Color Color::magenta		(255,   0, 191);
const Color Color::pink			(255,   0, 127);
const Color Color::crimson		(255,   0,  63);

///////////////////////////////////////////////////////////////////////////////

Color::Color() : m_red(0), m_green(0), m_blue(0)
{
}

Color::Color(int r, int g, int b) : m_red(r), m_green(g), m_blue(b)
{
}

Color::Color(const TCODColor& tc) : m_red(tc.r), m_green(tc.g), m_blue(tc.b)
{
}

Color::Color(const Color& copy) : m_red(copy.r()), m_green(copy.g()), m_blue(copy.b())
{
}

Color::~Color() {}

Color& Color::operator=(const Color& rhs)
{
	if (this != &rhs) {
		m_red = rhs.m_red;
		m_green = rhs.m_green;
		m_blue = rhs.m_blue;
	}

	return *this;
}

bool Color::operator==(const Color& rhs) const
{
	return ((m_red == rhs.m_red) &&
			(m_blue == rhs.m_blue) &&
			(m_green == rhs.m_green));
}

bool Color::operator!=(const Color& rhs) const
{
	return (false == operator==(rhs));
}

Color& Color::operator+=(const Color& rhs)
{
	if (this != &rhs) {
		m_red += rhs.m_red;
		m_blue += rhs.m_blue;
		m_green += rhs.m_green;
	}

	return *this;
}

Color& Color::operator-=(const Color& rhs)
{
	if (this != &rhs) {
		m_red -= rhs.m_red;
		m_blue -= rhs.m_blue;
		m_green -= rhs.m_green;
	}

	return *this;
}

Color& Color::operator+=(int rhs)
{
	m_red += rhs;
	m_blue += rhs;
	m_green += rhs;

	return *this;
}

Color& Color::operator-=(int rhs)
{
	m_red -= rhs;
	m_blue -= rhs;
	m_green -= rhs;

	return *this;
}

Color& Color::operator*=(int rhs)
{
	m_red *= rhs;
	m_blue *= rhs;
	m_green *= rhs;

	return *this;
}

Color& Color::operator/=(int rhs)
{
	if (rhs != 0) {
		m_red /= rhs;
		m_blue /= rhs;
		m_green /= rhs;
	}

	return *this;
}

Color& Color::operator*=(float rhs)
{
	m_red = (float)m_red * rhs;
	m_blue = (float)m_blue * rhs;
	m_green = (float)m_green * rhs;

	return *this;
}

Color& Color::operator/=(float rhs)
{
	if (rhs != 0) {
		m_red = (float)m_red / rhs;
		m_blue = (float)m_blue / rhs;
		m_green = (float)m_green / rhs;
	}

	return *this;
}

float Color::average() const
{
	float s = m_red + m_green + m_blue;
	return s / 3.0f;
}

void Color::normalize()
{
	float max = (float)std::max(m_red, std::max(m_green, m_blue));

	if (max <= 255) return;

	m_red = 255 * (float)m_red / max;
	m_green = 255 * (float)m_green / max;
	m_blue = 255 * (float)m_blue / max;
}

Color Color::normalize(const Color& c)
{
	Color ret = c;

	ret.normalize();
	return ret;
}

Color Color::greyscale(const Color& c, const GreyscaleType& type)
{
	Color ret = c;

	ret.greyscale(type);
	return ret;
}

void Color::greyscale(const GreyscaleType& type)
{
	int g;

	// first normalize
	normalize();

	switch (type) {
	case C_GREY_LIGHTNESS:
		// lightness
		g = (std::max(m_red, std::max(m_green, m_blue)) +
			 std::min(m_red, std::min(m_green, m_blue))) / 2;
		break;
	case C_GREY_LUMINOSITY:
		// average
		g = (m_red + m_blue + m_green) / 3;
		break;
	case C_GREY_AVERAGE:
		// luminosity
		g = 0.21f * (float)m_red + 0.72f * (float)m_green + 0.07f * (float)m_blue;
		break;
	}

	m_red = g;
	m_blue = g;
	m_green = g;
}

void Color::clamp(int lower, int upper)
{
	m_red = std::min(upper, std::max(lower, m_red));
	m_green = std::min(upper, std::max(lower, m_green));
	m_blue = std::min(upper, std::max(lower, m_blue));
}

void Color::clamp(const Color& lower, const Color& upper)
{
	m_red = std::min(upper.m_red, std::max(lower.m_red, m_red));
	m_green = std::min(upper.m_green, std::max(lower.m_green, m_green));
	m_blue = std::min(upper.m_blue, std::max(lower.m_blue, m_blue));
}

Color Color::clamp(const Color& base, int lower, int upper)
{
	return Color(std::min(upper, std::max(lower, base.m_red)),
				 std::min(upper, std::max(lower, base.m_green)),
				 std::min(upper, std::max(lower, base.m_blue)));
}

void Color::multiply(const Color& m)
{
	m_red *= (float)m.m_red / 255.0f;
	m_green *= (float)m.m_green / 255.0f;
	m_blue *= (float)m.m_blue / 255.0f;
}

Color Color::multiply(const Color& base, const Color& m)
{
	return Color(base.m_red * ((float)m.m_red / 255.0f),
				 base.m_green * ((float)m.m_green / 255.0f),
				 base.m_blue * ((float)m.m_blue / 255.0f));
}

void Color::lerp(const Color& to, float percent)
{
	m_red = m_red + (to.m_red - m_red) * percent;
	m_green = m_green + (to.m_green - m_green) * percent;
	m_blue = m_blue + (to.m_blue - m_blue) * percent;
}

Color Color::lerp(const Color& from, const Color& to, float percent)
{
	return Color(from.m_red + (to.m_red - from.m_red) * percent,
				 from.m_green + (to.m_green - from.m_green) * percent,
				 from.m_blue + (to.m_blue - from.m_blue) * percent);
}

void Color::augment(const Color& aug, float percent)
{
	m_red += aug.m_red * percent;
	m_green += aug.m_green * percent;
	m_blue += aug.m_blue * percent;
}

Color Color::augment(const Color& base, const Color& aug, float percent)
{
	return Color(base.m_red + aug.m_red * percent,
				 base.m_green + aug.m_green * percent,
				 base.m_blue + aug.m_blue * percent);
}

void Color::desaturate(float percent)
{
	float ave = (float)(m_red + m_blue + m_green) / 765.0f;
	float scale = (1.0f - percent) + (ave * percent);

	m_red *= scale;
	m_green *= scale;
	m_blue *= scale;
}

Color Color::desaturate(const Color& base, float percent)
{
	float ave = (float)(base.m_red + base.m_blue + base.m_green) / 765.0f;
	float scale = (1.0f - percent) + (ave * percent);

	return Color(base.m_red * scale,
				 base.m_green * scale,
				 base.m_blue * scale);
}

void Color::darken(float percent)
{
	desaturate(1.0f - percent);
	operator*=(percent);
}

Color Color::darken(const Color& base, float percent)
{
	return Color::desaturate(base, percent) * percent;
}

void Color::smooth()
{
	if (m_red > 255) m_red = sqrt((float)m_red / 255.0f) * 255;
	if (m_green > 255) m_green = sqrt((float)m_green / 255.0f) * 255;
	if (m_blue > 255) m_blue = sqrt((float)m_blue / 255.0f) * 255;
}

Color Color::smooth(const Color& base)
{
	Color ret = base;

	if (base.m_red > 255) ret.m_red = sqrt((float)base.m_red / 255.0f) * 255;
	if (base.m_green > 255) ret.m_green = sqrt((float)base.m_green / 255.0f) * 255;
	if (base.m_blue > 255) ret.m_blue = sqrt((float)base.m_blue / 255.0f) * 255;

	return ret;
}

Color operator+(const Color& lhs, const Color& rhs)
{
	return Color(lhs.r() + rhs.r(),
				 lhs.g() + rhs.g(),
				 lhs.b() + rhs.b());
}

Color operator+(const Color& lhs, int rhs)
{
	return Color(lhs.r() + rhs,
				 lhs.g() + rhs,
				 lhs.b() + rhs);
}

Color operator+(int lhs, const Color& rhs)
{
	return Color(lhs + rhs.r(),
				 lhs + rhs.g(),
				 lhs + rhs.b());
}

Color operator-(const Color& lhs, const Color& rhs)
{
	return Color(lhs.r() - rhs.r(),
				 lhs.g() - rhs.g(),
				 lhs.b() - rhs.b());
}

Color operator-(const Color& lhs, int rhs)
{
	return Color(lhs.r() - rhs,
				 lhs.g() - rhs,
				 lhs.b() - rhs);
}

Color operator-(int lhs, const Color& rhs)
{
	return Color(lhs - rhs.r(),
				 lhs - rhs.g(),
				 lhs - rhs.b());
}

Color operator*(const Color& lhs, float rhs)
{
	return Color((float)lhs.r() * rhs,
				 (float)lhs.g() * rhs,
				 (float)lhs.b() * rhs);
}

Color operator*(const Color& lhs, int rhs)
{
	return Color(lhs.r() * rhs,
				 lhs.g() * rhs,
				 lhs.b() * rhs);
}

Color operator*(float lhs, const Color& rhs)
{
	return Color(lhs * (float)rhs.r(),
				 lhs * (float)rhs.g(),
				 lhs * (float)rhs.b());
}

Color operator*(int lhs, const Color& rhs)
{
	return Color(lhs * rhs.r(),
				 lhs * rhs.g(),
				 lhs * rhs.b());
}

Color operator/(const Color& lhs, float rhs)
{
	return Color((float)lhs.r() / rhs,
				 (float)lhs.g() / rhs,
				 (float)lhs.b() / rhs);
}

Color operator/(const Color& lhs, int rhs)
{
	return Color(lhs.r() / rhs,
				 lhs.g() / rhs,
				 lhs.b() / rhs);
}

Color operator/(float lhs, const Color& rhs)
{
	return Color(lhs / (float)rhs.r(),
				 lhs / (float)rhs.g(),
				 lhs / (float)rhs.b());
}

Color operator/(int lhs, const Color& rhs)
{
	return Color(lhs / rhs.r(),
				 lhs / rhs.g(),
				 lhs / rhs.b());
}


Color gradient(int r)
{
	struct GradientPoint
	{
		Color c;
		float r;
	};

	static GradientPoint* colors;
	static const int NCOLORS = 4;

	int i0, i1;

	for (int i = 1; i < NCOLORS; i++) {
		i1 = i;
		i0 = i - 1;

		if (r < colors[i1].r) {
			break;
		}
	}

	float f = (r - colors[i0].r) / (colors[i1].r - colors[i0].r);

	return Color::lerp(colors[i0].c, colors[i1].c, f);
}

///////////////////////////////////////////////////////////////////////////////

Gradient::Gradient(const Color& a, const Color& b)
{
	GradientPoint first, last;

	first.m_color = a;
	first.m_value = 0.0f;
	last.m_color = b;
	last.m_value = 1.0f;

	m_points.insert(first);
	m_points.insert(last);
}

Gradient::Gradient(const Color& a, const Color& b, const Color& c)
{
	GradientPoint first, mid, last;

	first.m_color = a;
	first.m_value = 0.0f;
	mid.m_color = b;
	mid.m_value = 0.5f;
	last.m_color = c;
	last.m_value = 1.0f;

	m_points.insert(first);
	m_points.insert(mid);
	m_points.insert(last);
}

Gradient::Gradient(int ncolors, ...)
{
	if (ncolors > 0) {
		va_list vl;
		va_start(vl, ncolors);
		float v = 0.0f, inc = 1.0f / (float)(ncolors - 1);

		for (int i = 0; i < ncolors; i++) {
			GradientPoint p;

			p.m_value = v;
			p.m_color = va_arg(vl, Color);

			v += inc;
		}

		va_end(vl);
	}
}

Gradient::~Gradient()
{
}

void Gradient::addColor(const Color& c, float v)
{
	if (v <= 0.0f) return;
	if (v >= 1.0f) return;

	GradientPoint p;
	
	p.m_value = v;
	p.m_color = c;

	m_points.insert(p);
}

Color Gradient::getColor(float r) const
{
	std::set<GradientPoint>::const_iterator it = m_points.begin();

	if (m_points.size() == 0) return Color(0, 0, 0);

	float lastF = 0.0f;
	Color lastC = (*it).m_color;

	for (int i = 1; it != m_points.end(); it++, i++) {
		if (r < (*it).m_value) {
			float f = (r - lastF) / ((*it).m_value - lastF);

			return Color::lerp(lastC, (*it).m_color, f);
		}

		lastF = (*it).m_value;
		lastC = (*it).m_color;
	}

	return lastC;
}

Gradient& Gradient::operator=(const Gradient& rhs)
{
	if (this != &rhs) {
		m_points = rhs.m_points;
	}

	return *this;
}
