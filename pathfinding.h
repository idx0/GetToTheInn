#pragma once

#include "geometry.h"
#include "common.h"
#include "util.h"

#include <libtcod.hpp>
#include <queue>
#include <unordered_map>

struct PriorityQueue
{
	struct Location {
		int x, y;

		bool operator==(const Location& rhs) const
		{
			return ((x == rhs.x) && (y == rhs.y));
		}

		Location& operator=(const Location& rhs)
		{
			if (this != &rhs) {
				x = rhs.x;
				y = rhs.y;
			}
			return *this;
		}
		
		// compares lexographically
		bool operator<(const Location& rhs) const
		{
			if (x == rhs.x) {
				return (y < rhs.y);
			}
			return (x < rhs.x);
		}
		
		// compares lexographically
		bool operator>(const Location& rhs) const
		{
			if (x == rhs.x) {
				return (y > rhs.y);
			}
			return (x > rhs.x);
		}
	};

	template <class T> class LocationHash;

	template<> class LocationHash<PriorityQueue::Location>
	{
	public:
		std::size_t operator()(PriorityQueue::Location const& s) const
		{
			std::size_t hx = std::hash<int>()(s.x);
			std::size_t hy = std::hash<int>()(s.y);

			return hx ^ (hy << 1);
		}
	};

	typedef std::unordered_map<Location,
							   PriorityQueue::Location,
							   LocationHash<Location>> ParentMap;

	typedef std::unordered_map<Location,
							   float,
							   LocationHash<Location>> CostMap;

	typedef std::pair<float, Location> CostPair;

	std::priority_queue<CostPair, std::vector<CostPair>, std::greater<CostPair>> elements;
	
	inline bool empty()
	{
		return elements.empty();
	}

	inline void put(const Location& l, float cost)
	{
		elements.push(std::make_pair(cost, l));
	}

	inline void put(const Point& p, float cost)
	{
		Location l = { p.x(), p.y() };

		elements.push(std::make_pair(cost, l));
	}

	inline Location get()
	{
		Location best = elements.top().second;
		elements.pop();

		return best;
	}
};

class Pathfinder : public Utils::Singleton<Pathfinder>
{
public:

	Pathfinder();
	~Pathfinder();

	typedef void (*search_func)(PVector&, TheGrid*, const Point&, const Point&);

	static void dijkstra(PVector& path, TheGrid *grid, const Point& s, const Point& g);
	static void weights(WeightMap* map, TheGrid *grid);
};


///////////////////////////////////////////////////////////////////////////////
// This code is based off code by Joshua Day (https://github.com/joshuaday)
// and his progressive dijkstra scan

#include "viewport.h"

#define ALLOC_STATIC

#define PDS_OBSTRUCTION		-1
#define PDS_MAX_DISTANCE	30000

struct PDSNode
{
	short distance;
	short cost;

	PDSNode *prev;
	PDSNode *next;
};

class PDSQueue
{
public:
	PDSQueue() {}
	~PDSQueue() {}

	void clear();
	void scan(WeightMap* distance, WeightMap* cost);

	short farthest() { return m_farthest; }

protected:

	void update();
	void input(WeightMap* distance, WeightMap* cost);

protected:

	short m_farthest;
	PDSNode m_front;

#ifdef ALLOC_STATIC
	PDSNode m_list[WINDOW_WIDTH * WINDOW_HEIGHT];
#else
	PDSNode *m_list;
#endif
};
