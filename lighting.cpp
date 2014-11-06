#include "lighting.h"

#include "sys/logger.h"

const int LightingEngine::sMAX_LIGHT_LEVEL = 10;

///////////////////////////////////////////////////////////////////////////////

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

Color Color::greyscale(const Color& c)
{
	Color ret = c;

	ret.greyscale();
	return ret;
}

void Color::greyscale()
{
	// lightness
//	int g = (std::max(m_red, std::max(m_green, m_blue)) +
//			 std::min(m_red, std::min(m_green, m_blue))) / 2;

	// average
//	int g = (m_red + m_blue + m_green) / 3;

	// luminosity
	int g = 0.21f * (float)m_red + 0.72f * (float)m_green + 0.07f * (float)m_blue;

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

///////////////////////////////////////////////////////////////////////////////

void LightingEngine::apply_light(void *map, int x, int y, int dx, int dy, void *src)
{
	TheGrid* g = (TheGrid *)map;
	Light *l = (Light *)src;

	if ((g) && (l) && (g->inbounds(x, y))) {
		// the lighting calculate is based on a quadratic equation.  It provides
		// a nice look, while giving a nonlinear fall-off.
		// It is based on a simple formula
		//		L1 = L0 * (1 - dr^2 / rad^2)
		// Here, the resulting light level, L1, is given as a fraction of the original
		// light level, L0.  dr is the distance from the light source, equal to
		//		sqrt(dx^2 + dy^2); dx, dy from inputs
		// And rad is the radius of the light (literally l->radius).
		float dd = FSQR(l->radius);
		float dr = std::min(dd, FSQR(dx) + FSQR(dy));
		float lc = g->get(x, y)->render->lighting.lightLevel;

		float coef = (1 - (dr / dd));

		float ln = sqrt(FSQR(lc) + FSQR(l->lightLevel * coef));

		g->at(x, y)->render->lighting.lightColor += l->color * coef * l->lightLevel;

//		sys::logger::log("color := [%03d,%03d,%03d]",
//			g->at(x, y)->render->lighting.lightColor.r(),
//			g->at(x, y)->render->lighting.lightColor.g(),
//			g->at(x, y)->render->lighting.lightColor.b());

//		printf("@(%d,%d) := [%d, %d] -> %3.3f [%3.3f]\n", x, y, dx, dy, dr, ln);

		g->at(x, y)->render->lighting.lightLevel = ln;
	}
}

int LightingEngine::opaque(void *map, int x, int y)
{
	TheGrid* g = (TheGrid *)map;

	if ((g) && (g->inbounds(x, y))) {
		return (g->get(x, y)->render->lighting.transparent ? FOV_FALSE : FOV_TRUE);
	}

	return FOV_TRUE;
}

///////////////////////////////////////////////////////////////////////////////

Light::Light(int x, int y, float level, int rad, const Color& c) :
	position(Point(x, y)),
	color(c),
	lightLevel(level),
	radius(rad)
{
	ray.angle = 0.0f;
	ray.direction = FOV_NORTH;
	ray.enabled = false;

	fov_settings_init(&m_fov_settings);
	fov_settings_set_opacity_test_function(&m_fov_settings, LightingEngine::opaque);
    fov_settings_set_apply_lighting_function(&m_fov_settings, LightingEngine::apply_light);

	LightingEngine::getInstance()->addLight(this);
}

Light::~Light()
{
	LightingEngine::getInstance()->removeLight(this);
	fov_settings_free(&m_fov_settings);
}

void Light::calculateLighting(TheGrid* grid)
{
	if (ray.enabled) {
		fov_beam(&m_fov_settings,
				 grid, this,
				 position.x(),
				 position.y(),
				 radius,
				 ray.direction,
				 ray.angle);
	} else {
		fov_circle(&m_fov_settings,
				   grid, this,
				   position.x(),
				   position.y(),
				   radius);
	}
}

///////////////////////////////////////////////////////////////////////////////

void LightingEngine::addLight(Light *l)
{
	m_lights.insert(l);
}

void LightingEngine::removeLight(Light *l)
{
	m_lights.erase(l);
}

void LightingEngine::calculateLighting(TheGrid* grid, const Rect& renderRect)
{
	std::set<Light*>::iterator it = m_lights.begin();

	while (it != m_lights.end()) {
		if (renderRect.inbounds((*it)->position)) {
			(*it)->calculateLighting(grid);
		}

		it++;
	}
}
