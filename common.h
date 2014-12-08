#pragma once

#include <vector>
#include <assert.h>
#include "libtcod/libtcod.hpp"

#include "sys/thread_os.h"
#include "geometry.h"
#include "delay.h"
#include "color.h"

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

enum MoibilityFlags
{
	M_WALKABLE		= B1(0),	// tile can be walked on by monsters and the player
	M_DAMAGING		= B1(1),	// this tile may damage things that step on it in some way
	M_JUMPABLE		= B1(2),	// this tile can be jumped, flown, or hovered over
	M_REACHABLE		= B1(3),	// monsters/player can reach objects on this tile from adjancent
								// tiles
};


class MobilityModel : public Model
{
public:
	MobilityModel() : flags(0) {}
	~MobilityModel() {}

	unsigned int flags;

	void reset()
	{
		// nothing here is reset
	}
};

enum DiscoveryFlags
{
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

	D_VISIBLE			= (D_SEEN | D_CLAIRVOYANT | D_TELEPATHIC),
	D_ALREADY_KNOWN		= (D_EXPLORED | D_MAPPED | D_MAGIC_MAPPED),
};

class DiscoveryModel : public Model
{
public:
	DiscoveryModel() : flags(0) {}
	~DiscoveryModel() {}

	// true if within player viewing range
	unsigned int flags;

	void reset()
	{
		static const unsigned int RESET_BITS = D_HIDDEN | D_SEEN | D_HEARD;
		flags &= ~RESET_BITS;
	}
};

enum LightingFlags
{
	L_LIT				= B1(0),	// this tile was lit by the lighting engine
	L_TRANSPARENT		= B1(1),	// light can shine through this tile
	L_ALWAYS_LIT		= B1(2),	// this tile 's fg is always lit when visible
	L_IN_SHADOW			= B1(3),	// this tile is in shadow
	L_ENSURE_CONTRAST	= B1(4),	// this tile's fg color should be adjusted when
									// too close to the tile's bg color
};

class LightingModel : public Model
{
public:
	LightingModel() : flags(0) {}
	~LightingModel() {}

	unsigned int flags;

	Color ambientColor;
	Color lightColor;

	void reset()
	{
		static const unsigned int RESET_BITS = L_LIT;

		flags &= ~RESET_BITS;
		lightColor = ambientColor;
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
