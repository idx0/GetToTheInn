#pragma once

#include <vector>
#include <assert.h>
#include <libtcod.hpp>

#include "sys/thread_os.h"
#include "geometry.h"
#include "delay.h"

#define THREADS
//#define USE_CURSOR

#define STATUS_HEIGHT	3
#define HEADER_SPACE	0
#define COLORED_LIGHTING

#ifdef USE_CURSOR
#define FPS	60
#else
#define FPS 30
#endif

#define B1(x) (1 << (x))
#define FSQR(x) (float)((x) * (x))

///////////////////////////////////////////////////////////////////////////////

class Color
{
public:
	Color();
	Color(int r, int g, int b);
	Color(const TCODColor& tc);
	Color(const Color& copy);

	~Color();

	Color& operator=(const Color& rhs);
	bool operator==(const Color& rhs) const;
	bool operator!=(const Color& rhs) const;

	Color& operator+=(const Color& rhs);
	Color& operator-=(const Color& rhs);

	Color& operator+=(int rhs);
	Color& operator-=(int rhs);

	Color& operator*=(int rhs);
	Color& operator/=(int rhs);
	Color& operator*=(float rhs);
	Color& operator/=(float rhs);

	int r() const;
	int g() const;
	int b() const;

	// normalizes the color
	void normalize();
	static Color normalize(const Color& c);

	// returns a greyscale of the given color
	static Color greyscale(const Color& c);
	void greyscale();

	// returns the clamped color (when lighting is applied, a color may be light
	// above its allowable value)
	//    this.n = std::min(upper, std::max(lower, this.n));
	void clamp(int lower = 0, int upper = 255);
	void clamp(const Color& lower, const Color& upper);
	static Color clamp(const Color& base, int lower = 0, int upper = 255);

	// multiplies this color by the given color
	//   this.n *= m.n / 255;
	void multiply(const Color& m);
	static Color multiply(const Color& base, const Color& m);

	// linear interpolation between two colors
	//   this.n = this.n + (to.n - this.n) * percent;
	void lerp(const Color& to, float percent);
	static Color lerp(const Color& from, const Color& to, float percent);

	// "augments" this color by the weighted value of aug
	//   this.n += aug.n * percent;
	void augment(const Color& aug, float percent);
	static Color augment(const Color& base, const Color& aug, float percent);

	// desaturates the color by the given percentage
	//   float ave = (this.red + this.blue + this.green) / (3 * 255);
	//   this.n *= (1.0f - percent) + (ave * percent);
	void desaturate(float percent);
	static Color desaturate(const Color& base, float percent);

	// this operation returns a darkened version of this color based on
	// percent.  Here, percent represents how much of the original color
	// is retained.  At 1.0f - this function returns the color, while at
	// 0.0f, this function returns black.
	void darken(float percent);
	static Color darken(const Color& base, float percent);

	// smooths the color 
	//   if (this.n > 255) this.n = sqrt(this.n / 255) * 255;
	void smooth();
	static Color smooth(const Color& base);

	TCODColor toTCOD() const;

public:

	// monochrome
	static const Color white;
	static const Color black;

	// greys
	static const Color grey;
	static const Color slate;

	// sepiatone
	static const Color sepia;
	static const Color sienna;
	static const Color brown;

	// misc named
	static const Color celadon;
	static const Color peach;

	// metalic
	static const Color brass;
	static const Color copper;
	static const Color gold;
	static const Color silver;

	// rainbow
	static const Color red;
	static const Color flame;
	static const Color orange;
	static const Color amber;
	static const Color yellow;
	static const Color neon;
	static const Color chartreuse;
	static const Color lime;
	static const Color green;
	static const Color mint;
	static const Color sea;
	static const Color turquise;
	static const Color cyan;
	static const Color sky;
	static const Color azure;
	static const Color navy;
	static const Color blue;
	static const Color han;
	static const Color violet;
	static const Color purple;
	static const Color fuchsia;
	static const Color magenta;
	static const Color pink;
	static const Color crimson;

protected:

	int m_red;
	int m_green;
	int m_blue;
};

inline
int Color::r() const
{
	return m_red;
}

inline
int Color::g() const
{
	return m_green;
}

inline
int Color::b() const
{
	return m_blue;
}

inline
TCODColor Color::toTCOD() const
{
	return TCODColor(m_red, m_green, m_blue);
}

Color operator+(const Color& lhs, const Color& rhs);
Color operator+(const Color& lhs, int rhs);
Color operator+(int lhs, const Color& rhs);

Color operator-(const Color& lhs, const Color& rhs);
Color operator-(const Color& lhs, int rhs);
Color operator-(int lhs, const Color& rhs);

Color operator*(const Color& lhs, float rhs);
Color operator*(const Color& lhs, int rhs);
Color operator*(float lhs, const Color& rhs);
Color operator*(int lhs, const Color& rhs);

Color operator/(const Color& lhs, float rhs);
Color operator/(const Color& lhs, int rhs);
Color operator/(float lhs, const Color& rhs);
Color operator/(int lhs, const Color& rhs);

///////////////////////////////////////////////////////////////////////////////

class Tile
{
public:
	Tile() :
		fgColor(Color::black),
		bgColor(Color::black),
		icon(' ') {}
	Tile(const Color& c, int i) :
		fgColor(c),
		bgColor(Color::black),
		icon(i) {}
	Tile(const Tile& rhs) { operator=(rhs); }

	Tile& operator=(const Tile& rhs);

	Color fgColor;
	Color bgColor;
	int icon;
};

class Model
{
public:
	Model() {}
	virtual ~Model() {}

	virtual void reset() = 0;
};


class MobilityModel : public Model
{
public:
	MobilityModel() : walkable(false), reachable(false), jumpable(false) {}
	~MobilityModel() {}

	// if this is true, the player can walk on this tile
	bool walkable;

	// if this is true, the player can reach objects on this tile without
	// having to actually be on the tile
	bool reachable;

	// if this is true, the player can jump onto or off of this tile
	bool jumpable;


	void reset()
	{
		// nothing here is reset
	}
};

enum DiscoveryFlags {
	D_DISCOVERED		= B1(0),	// tile was discovered by the player
	D_SEEN				= B1(1),	// tile is seen by the player
	D_EXPLORED			= B1(2),	// tile was seen by the player
	D_HIDDEN			= B1(3),	// tile is hidden to the player
	D_HEARD				= B1(4),	// tile is within the player hearing range
	D_MAPPED			= B1(5),	// tile is mapped by a found map
	D_MAGIC_MAPPED		= B1(6),	// tile is mapped by a magic item
	D_CLAIRVOYANT		= B1(7),	// tile is visible via clairvoyance
	D_TELEPATHIC		= B1(8),	// tile is visible via telepathy with other entities
	D_FELT				= B1(9),	// tile is felt by the player
};

class DiscoveryModel : public Model
{
public:
	DiscoveryModel() : seen(false), hidden(false), heard(false), explored(false) {}
	~DiscoveryModel() {}

	// true if within player viewing range
	bool seen;

	// true if seen, but hidden by darkness
	bool hidden;

	// true if within player hearing range
	bool heard;

	// true if this position have been previously seen
	bool explored;

	void reset()
	{
		hidden = false;
		seen = false;
		heard = false;
	}
};

enum LightingFlags {
	L_LIT				= B1(0),	// this tile was lit by the lighting engine
	L_TRANSPARENT		= B1(1),	// light can shine through this tile
	L_ALWAYS_LIT		= B1(2),	// this tile 's fg is always lit when visible
	L_IN_SHADOW			= B1(3),	// this tile is in shadow
};

class LightingModel : public Model
{
public:
	LightingModel() : fullIfVisible(false), transparent(true),
					  lightLevel(0.0f), ambientLevel(0.0f),
					  ambientColor(), lightColor() {}
	~LightingModel() {}

	// true if this object should be shown in full color
	// if visible at all (useful for intractable objects, or
	// objects which give off light)
	bool fullIfVisible;

	bool transparent;

	float lightLevel;

	float ambientLevel;

	Color ambientColor;
	Color lightColor;

	void reset()
	{
		lightColor = ambientColor;
		lightLevel = ambientLevel;
	}
};


class TemperatureModel : public Model
{
public:
	TemperatureModel() : ambient(0), temp(0) {}
	~TemperatureModel() {}

	// temperature is a scale from -10 to 10.  The temperature values abs(temp) behave
	// according to the following chart
	//   0-3	normal
	//   4-6	tile reported as "warm" or "cool"
	//   7-8	tile reported as "hot" or "cold"
	//   9+		cause damage based to objects in this tile based (fire for hot,
	//			water for cold), adjusted for resistances
	//
	// for all values above 3, the tile bg is colored

	// the ambient temperature on this tile
	int ambient;

	// the current temperature on this tile
	int temp;

	// reset simply resets the temperature to ambient, in game, things can
	// change the temperature of tiles, in which case they should gradually
	// fall back toward the ambient over time
	void reset()
	{
		temp = ambient;
	}

	void regress()
	{
		int z = (ambient - temp) / 2;
		temp += z;
	}
};


template<typename T>
class ModelList
{
public:
	ModelList(int w, int h) : m_width(w), m_height(h)
	{
		m_list = new (std::nothrow) T[w * h];
		assert(m_list);
	}

	~ModelList()
	{
		delete[] m_list;
	}

	int width() const
	{
		return m_width;
	}

	int height() const
	{
		return m_height;
	}

	Size size() const
	{
		return Size(m_width, m_height);
	}

	size_t bytes() const
	{
		return (sizeof(T) & size().area());
	}

	const T* get(int x, int y) const
	{
		if (inbounds(x, y)) {
			return &(m_list[x + y * m_width]);
		}
		return static_cast<T*>(0);
	}

	const T* get(const Point& p) const
	{
		return get(p.x(), p.y());
	}

	T* at(int x, int y)
	{
		if (inbounds(x, y)) {
			return &(m_list[x + y * m_width]);
		}
		return static_cast<T*>(0);
	}

	T* at(const Point& p)
	{
		return at(p.x(), p.y());
	}

	T* list() { return m_list; }

	inline
	bool inbounds(int x, int y) const
	{
		return (((x < m_width) && (y < m_height)) &&
				((x >= 0) && (y >= 0)));
	}

	inline
	bool inbounds(const Point& p) const
	{
		return inbounds(p.x(), p.y());
	}

	void fill(const T& t)
	{
		for (int x = 0; x < m_width; x++) {
			for (int y = 0; y < m_height; y++) {
				m_list[x + m_width * y] = t;
			}
		}
	}

	ModelList<T>* copy() const
	{
		ModelList<T>* ret = new ModelList<T>(m_width, m_height);

		memcpy(ret->m_list, m_list, sizeof(T) * m_width * m_height);

		return ret;
	}

	void copy(const ModelList<T>* rhs)
	{
		memcpy(m_list, rhs->m_list, sizeof(T) * m_width * m_height);
	}

	void copy(const ModelList<T>* rhs, const Rect& offset)
	{
		Rect b(0, 0, m_height, m_width);

		if (!b.inbounds(offset)) return;

		// copy by row
		for (int r = 0; r < offset.height(); r++) {
			int s0 = offset.left() + offset.width() * r;
			int s1 = r * offset.width();

			memcpy(m_list + s0, rhs->m_list + s1, sizeof(T) * offset.width());
		}
	}

protected:

	int m_width;
	int m_height;

	T* m_list;
};


typedef ModelList<DiscoveryModel> DiscoveryList;

typedef ModelList<MobilityModel> MobilityList;

typedef ModelList<LightingModel> LightingList;

typedef ModelList<TemperatureModel> TemperatureList;

// an array of shorts we can use for weightmaps
typedef ModelList<short> WeightMap;

///////////////////////////////////////////////////////////////////////////////

class Object;

struct RenderGrid
{
	Tile tile;
	LightingModel lighting;
	DiscoveryModel discover;
	MobilityModel mobility;
	TemperatureModel temp;
};

typedef ModelList<RenderGrid> RenderSettings;

struct Grid
{
	RenderGrid* render;

	Object* mapObj;
	Object* dynObj;

	Delay* delay;

	Grid() : render(NULL), mapObj(NULL), dynObj(NULL), delay(NULL) {}

	void update();
};

typedef ModelList<Grid> TheGrid;

///////////////////////////////////////////////////////////////////////////////
