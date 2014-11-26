#pragma once

#include <libtcod/libtcod.hpp>

#include "geometry.h"
#include "fov/fov.h"
#include "common.h"
#include "lighting.h"
#include "object.h"

#include <string>
#include <vector>
#include <stdio.h>

#define HP_UNKNOWN		-1
#define HP_DEAD			-2
#define HP_UNBREAKABLE	-3
#define HP_INVULNERABLE	-4


class Dirt : public Object
{
public:
	Dirt(int x, int y);
};

class Wall : public Object
{
public:
   Wall(int x, int y);
};

class FloraWall : public Object
{
public:
	FloraWall(const Point& p);
};

class Grass : public Object
{
public:
	Grass(const Point& p);
};


class Torch : public Object
{
public:
	Torch(int x, int y, int level, int rad);
	~Torch();
};

///////////////////////////////////////////////////////////////////////////////

//#define MAP_WIDTH	1024
//#define MAP_HEIGHT	512
#define MAP_WIDTH	96
#define MAP_HEIGHT	72

class Map
{
public:
	Map(int w, int h);
	~Map();

	void generateMap();

	bool inbounds(int x, int y) const;

	bool isWall(int x, int y) const;
	void setWall(int x, int y, bool iswall = true);

	int width() const;
	int height() const;

	Tile tile(int x, int y);
	Object* staticObject(int x, int y);

	const MobilityList* staticCopy();

	// finds a random free point on the map at least d cells away from any
	// wall - Point(-1, -1) is returned if no cell can be found that meets
	// the given criteria
	Point findFree(int d);

	// finds a random free point on the map on or around the given point
	// at least d cells away from any wall - Point(-1, -1) is returned if
	// no cell can be found that meets the given criteria
	Point findNear(const Point& o, int d);

protected:

	// removes diagonal pieces by randomly removing or adding a wall
	//
	//  .#.    ##.    .#.    ...
	//  #.. -> #.. or ... or #..
	//  ...    ...    ...    ...
	//
	void removeDiagonals();

	void generateRooms(int mw = 10, int mh = 10);

	// generates a map based on game-of-life style cellular automata.
	//    The B and S variables are arrays of integers which specify the parameters
	//    for generation.  Blen and Slen are the array lengths respectively.  The
	//    arrays define the number of neighbors for generation.  So to generate a nice
	//    maze with B3/S1234, you would define:
	//
	//		int B[] = { 3 };
	//		int Blen = 1;
	//		int S[] = { 1, 2, 3, 4 }
	//		int Slen = 4;
	//		generateLife(B, Blen, S, Slen);		
	//
	struct Rule
	{
		std::vector<int> B;
		std::vector<int> S;

		Rule& operator=(const Rule& rhs)
		{
			if (this != &rhs) {
				B = rhs.B;
				S = rhs.S;
			}

			return *this;
		}

		void print() const
		{
			unsigned int i;

			printf("B");
			for (i = 0; i < B.size(); i++) { printf("%d", B[i]); }
			printf("/S");
			for (i = 0; i < S.size(); i++) { printf("%d", S[i]); }
			printf("\n");
		}
	};

	static const std::string Rules[];
	static Rule fromString(const std::string& rule);

	void generateLife(const Rule& rule, float coef = 0.3, int iter = 8);

	int countNeighbors(int x, int y) const;

	enum _neighbor { unlinked, linked, none };
	enum _dir { north = 0, south, east, west };

	struct room {
		_neighbor n[4];
		bool visited;
	};

	void linkRooms(
		const int ncol, const int nrow,
		room* rooms, int *visited,
		int pc, int pr);

	void placeRandomTorches(ObjectMap& dynamObj);

protected:
	int m_width;
	int m_height;

	void dig(int x1, int y1, int x2, int y2);
	void dig(const Rect& r);
	void dig(const Point& p1, const Point& p2);
	void dig(const Point& p);
	void dig(int x, int y);

	void fill(int x1, int y1, int x2, int y2);
	void fill(const Rect& r);
	void fill(const Point& p1, const Point& p2);
	void fill(const Point& p);
	void fill(int x, int y);

	// fills the whole map
	void fill();
	// digs the whole map
	void dig();

	// randomly fills coef% of the map (0 <= coef <= 1)
	void fill(float coef);

	fov_settings_type m_fov_settings;

	MobilityList m_grid;
	ObjectMap m_staticObjects;

	// a weight map for the distance from any wall
	WeightMap m_distMap;

	// the farthest any cell is from a wall
	int m_farthest;
};

inline
bool Map::inbounds(int x, int y) const
{
	return (((x < m_width) && (y < m_height)) &&
			((x >= 0) && (y >= 0)));
}
