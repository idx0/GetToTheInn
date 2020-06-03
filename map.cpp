#include <algorithm>

#include "map.h"
#include "engine.h"
#include "player.h"
#include "rnd.h"
#include "pathfinding.h"

#include "sys/logger.h"

//#define FLORA

Dirt::Dirt(int x, int y) : Object(x, y, ' ', gtti::Color::black)
{
	static int icons[3] = { '.', 249, 250 };

	//litColor	= TCODColor::lerp(TCODColor::darkSepia, TCODColor::desaturatedGreen, Rnd::rndn());
	

    gtti::Color fg = gtti::Color::lerp(gtti::Color(94, 75, 47), gtti::Color(63, 127, 95), Rnd::rndn());
	fg.scaleHSV(Rnd::betweenf(0.6f, 1.0f), Rnd::betweenf(0.6f, 1.0f));

	m_fgColor = gtti::Color(fg);
	m_bgColor = gtti::Color(gtti::Color::lerp(gtti::Color(12, 8, 4), gtti::Color(0, 8, 4), Rnd::rndn()));
	
	m_icon = icons[Rnd::between(0, 3)];

	m_mobilityModel.flags |= (M_WALKABLE | M_JUMPABLE | M_REACHABLE);
	m_lightingModel.flags |= L_TRANSPARENT;

	m_flavor = std::string("the ground");
}

Grass::Grass(const Point& p) : Object(p, ' ', gtti::Color::black)
{
	static int icons[4] = { ',', '`', '\'', '"' };

	m_fgColor = gtti::Color::lerp(gtti::Color(25, 31, 12), gtti::Color(25, 62, 12), Rnd::rndn());
	m_bgColor = gtti::Color::lerp(gtti::Color(6, 6, 0), gtti::Color(0, 12, 0), Rnd::betweenf(0.3f, 0.6f));

	m_icon = icons[Rnd::between(0, 4)];

	m_mobilityModel.flags |= (M_WALKABLE | M_JUMPABLE | M_REACHABLE);
	m_lightingModel.flags |= L_TRANSPARENT;

	m_flavor = std::string("grassy turf");
}


//Wall::Wall(int x, int y) : Object(x, y, '#', TCODColor::amber)
Wall::Wall(int x, int y) : Object(x, y, '#', gtti::Color::lerp(gtti::Color::grey, gtti::Color::azure, 0.30f))
{
	m_bgColor = gtti::Color::lerp(gtti::Color(12, 12, 12), gtti::Color(0, 12, 24), 0.40f);

	m_flavor = std::string("a rough stone wall");
}

FloraWall::FloraWall(const Point& p) : Object(p, '#', gtti::Color(121, 110, 64))
{
	m_fgColor = gtti::Color::lerp(gtti::Color(35, 41, 22), gtti::Color(59, 77, 22), Rnd::rndn());
	m_bgColor = gtti::Color::lerp(gtti::Color(6, 6, 0), gtti::Color(0, 12, 0), Rnd::betweenf(0.3f, 0.6f));

	m_flavor = std::string("an overgrowth of roots, vines, and moss cover a stone wall");
}

Torch::Torch(int x, int y, int level, int rad) : NamedObject("torch", x, y, 'i', gtti::Color::gold)
{
	m_light = new Light(x, y, level, rad);

	m_mobilityModel.flags |= (M_JUMPABLE | M_REACHABLE);

	m_lightingModel.ambientColor = gtti::Color(96, 96, 96);
	m_lightingModel.flags |= L_ALWAYS_LIT;
}

Torch::~Torch()
{
	delete m_light;
}

///////////////////////////////////////////////////////////////////////////////

Tile& Tile::operator=(const Tile& rhs)
{
	if (this != &rhs) {
		icon	= rhs.icon;
		fgColor	= rhs.fgColor;
		bgColor	= rhs.bgColor;
	}

	return *this;
}

///////////////////////////////////////////////////////////////////////////////

const std::string Map::Rules[] =
{
	"B1/S1",			//  0 gnarl
	"B1357/S1357",		//  1 replicator
	"B1357/S02648",		//  2 fredkin
	"B2/S",				//  3 seeds
	"B2/S0",			//  4 lfod
	"B234/S",			//  5 serviettes
	"B3/S023",			//  6 dotlife
	"B3/S012345678",	//  7 lwod
	"B3/S1234",			//  8 mazectric
	"B3/S12345",		//  9 maze
	"B3/S23",			// 10 life
	"B3/S45678",		// 11 coral
	"B34/S34",			// 12 34life
	"B345/S4567",		// 13 assimilation
    "B345/S5",			// 14 longlife
	"B35678/S5678",		// 15 diamoeba
	"B357/S1358",		// 16 amoeba
	"B357/S238",		// 17 pseudolife
	"B36/S125",			// 18 2x2
	"B36/S23",			// 19 highlife
	"B368/S245",		// 20 move
	"B3678/S235678",	// 21 stains
	"B3678/S34678",		// 22 daynight
	"B37/S23",			// 23 drylife
	"B378/S235678",		// 24 coagulations
	"B45678/S2345",		// 25 walledcities
	"B4678/S35678",		// 26 vote45
	"B5678/S45678",		// 27 vote
	"B0123478/S34678",	// 28 inverselife
	"B5678/S45678"		// 29 45rule
};


Map::Rule Map::fromString(const std::string& rule)
{
	bool bmode = false;
	Rule ret;

	// parse
	for (int i = 0; i < (int)rule.length(); i++) {
		char c = rule.at(i);
		int z = -1;

		if (c == 'B') {
			bmode = true;
		} else if (c == 'S') {
			bmode = false;
		} else if (c != '/') {
			z = c - '0';

			if (bmode) {
				ret.B.push_back(z);
			} else {
				ret.S.push_back(z);
			}
		}
	}

	return ret;
}


Map::Map(int w, int h) :
	m_width(w), m_height(h),
	m_grid(w, h),
	m_staticObjects(w * h, static_cast<Object*>(0)),
	m_distMap(w, h)
{
	// initialize map
#if 0
	dig(20, 20, 50, 50);
	fill(25, 25);
	fill(25, 30);
	fill(25, 35);
	fill(25, 40);
	fill(25, 45);
	m_lights.push_back(new Light(26, 35, 6, 8));
#endif
	
}

void Map::generateMap()
{
//	generateLife(fromString(Rules[8]), 0.4f, 125);

	generateLife(fromString("B5678/S45678"), 0.55f, 1);
	removeDiagonals();

	WeightMap costMap(m_grid.width(), m_grid.height());

	for (int x = 0; x < m_grid.width(); x++) {
		for (int y = 0; y < m_grid.height(); y++) {
			*(costMap.at(x, y)) = 1;

			if (isWall(x, y)) {
				*(m_distMap.at(x, y)) = 0;
			} else {
				*(m_distMap.at(x, y)) = PDS_MAX_DISTANCE;
			}
		}
	}

	PDSQueue q;
	q.scan(&m_distMap, &costMap);

	m_farthest = q.farthest();

//	generateRooms();
//	placeRandomTorches(dynamObj);
}


Map::~Map()
{
	fov_settings_free(&m_fov_settings);
}

bool Map::isWall(int x, int y) const
{
	if (inbounds(x, y)) {
		return !(m_grid.get(x, y)->flags & M_WALKABLE);
	}

	return false;
}

void Map::setWall(int x, int y, bool iswall)
{
	int i = x + y * m_width;

	if (inbounds(x, y)) {
		delete m_staticObjects[i];

		if (iswall) {
			m_staticObjects[i] = new Wall(x, y);
		} else {
			m_staticObjects[i] = new Dirt(x, y);
		}

		MobilityModel *m = m_grid.list();
		m[i] = m_staticObjects[i]->mobilityModel();
	}
}

void Map::overgrow(int x, int y, int around)
{
    int i = x + y * m_width;
    bool placed = false;

    if (inbounds(x, y) && i < m_staticObjects.size()) {
        Object *obj = m_staticObjects[i];

        if (dynamic_cast<Wall*>(obj)) {
            m_staticObjects[i] = new FloraWall(Point(x, y));
            placed = true;
            delete obj;
        }
        else if (dynamic_cast<Dirt*>(obj)) {
            m_staticObjects[i] = new Grass(Point(x, y));
            placed = true;
            delete obj;
        }

        if (!placed) {
            return;
        }

        MobilityModel *m = m_grid.list();
        m[i] = m_staticObjects[i]->mobilityModel();

        if (around > 1) {
            around--;

            overgrow(x, y + 1, around);
            overgrow(x - 1, y, around);
            overgrow(x + 1, y, around);
            overgrow(x, y - 1, around);
        }
    }
}

int Map::width() const
{
	return m_width;
}

int Map::height() const
{
	return m_height;
}

Point Map::findFree(int d)
{
	Point p;

	do {
		p = Point(Rnd::between(0, m_width),
				  Rnd::between(0, m_height));
	} while (!isWall(p.x(), p.y()));

	return findNear(p, d);
}

Point Map::findNear(const Point& o, int d)
{
	Point p = o, last = o;

	if (d > m_farthest) return Point(-1, -1);
	if (!m_distMap.inbounds(o)) return Point(-1, -1);

	short dist = *(m_distMap.get(p));
	int idir;

	while (dist < d) {
		// generate a random neighbor to search first, because we
		// don't want to randomly walk consistantly
		int rdir = Rnd::between(0, NNEIGHBORS);

		// find a higher neighbor
		for (idir = 0; idir < NNEIGHBORS; idir++) {
			int dir = (rdir + idir) % NNEIGHBORS;
			Point n = p + Point(NEIGHBORS[dir].dx, NEIGHBORS[dir].dy);

			// dont go back
			if (n == last) continue;
			if (m_distMap.inbounds(n)) {
				short ndist = *(m_distMap.get(n));

				if ((!isWall(n.x(), n.y())) &&
					(ndist > dist))
				{
					p = n;
					break;
				}
			}
		}

		if (idir == NNEIGHBORS) {
			// we are at a local maximum, move a random direction
			do {
				int dir = Rnd::between(0, NNEIGHBORS);
				p += Point(NEIGHBORS[dir].dx, NEIGHBORS[dir].dy);
			} while (!isWall(p.x(), p.y()));
		}
		
		dist = *(m_distMap.get(p));
		last = p;
	}

	return p;
}

void Map::removeDiagonals()
{
	// skip the first and last row/col
	for (int x = 1; x < m_grid.width() - 2; x++) {
		for (int y = 1; y < m_grid.height() - 2; y++) {

			if ((isWall(x + NEIGHBORS[EAST].dx, y + NEIGHBORS[EAST].dy)) &&				// ...
				(isWall(x + NEIGHBORS[SOUTH].dx, y + NEIGHBORS[SOUTH].dy)) &&			// ..#
				(!isWall(x + NEIGHBORS[SOUTHEAST].dx, y + NEIGHBORS[SOUTHEAST].dy))) {	// .#.
				int c = Rnd::between(0, 4);
				switch (c) {
				default: setWall(x + NEIGHBORS[SOUTHEAST].dx, y + NEIGHBORS[SOUTHEAST].dy, true); break;
				case 1: setWall(x + NEIGHBORS[SOUTH].dx, y + NEIGHBORS[SOUTH].dy, false); break;
				case 2: setWall(x + NEIGHBORS[EAST].dx, y + NEIGHBORS[EAST].dy, false); break;
				}
			}

			if ((isWall(x + NEIGHBORS[EAST].dx, y + NEIGHBORS[EAST].dy)) &&				// .#.
				(isWall(x + NEIGHBORS[NORTH].dx, y + NEIGHBORS[NORTH].dy)) &&			// ..#
				(!isWall(x + NEIGHBORS[NORTHEAST].dx, y + NEIGHBORS[NORTHEAST].dy))) {	// ...
				int c = Rnd::between(0, 4);
				switch (c) {
				default: setWall(x + NEIGHBORS[NORTHEAST].dx, y + NEIGHBORS[NORTHEAST].dy, true); break;
				case 1: setWall(x + NEIGHBORS[NORTH].dx, y + NEIGHBORS[NORTH].dy, false); break;
				case 2: setWall(x + NEIGHBORS[EAST].dx, y + NEIGHBORS[EAST].dy, false); break;
				}
			}

			if ((isWall(x + NEIGHBORS[WEST].dx, y + NEIGHBORS[WEST].dy)) &&				// ...
				(isWall(x + NEIGHBORS[SOUTH].dx, y + NEIGHBORS[SOUTH].dy)) &&			// #..
				(!isWall(x + NEIGHBORS[SOUTHWEST].dx, y + NEIGHBORS[SOUTHWEST].dy))) {	// .#.
				int c = Rnd::between(0, 4);
				switch (c) {
				default: setWall(x + NEIGHBORS[SOUTHWEST].dx, y + NEIGHBORS[SOUTHWEST].dy, true); break;
				case 1: setWall(x + NEIGHBORS[SOUTH].dx, y + NEIGHBORS[SOUTH].dy, false); break;
				case 2: setWall(x + NEIGHBORS[WEST].dx, y + NEIGHBORS[WEST].dy, false); break;
				}
			}

			if ((isWall(x + NEIGHBORS[WEST].dx, y + NEIGHBORS[WEST].dy)) &&				// .#.
				(isWall(x + NEIGHBORS[NORTH].dx, y + NEIGHBORS[NORTH].dy)) &&			// #..
				(!isWall(x + NEIGHBORS[NORTHWEST].dx, y + NEIGHBORS[NORTHWEST].dy))) {	// ...
				int c = Rnd::between(0, 4);
				switch (c) {
				default: setWall(x + NEIGHBORS[NORTHWEST].dx, y + NEIGHBORS[NORTHWEST].dy, true); break;
				case 1: setWall(x + NEIGHBORS[NORTH].dx, y + NEIGHBORS[NORTH].dy, false); break;
				case 2: setWall(x + NEIGHBORS[WEST].dx, y + NEIGHBORS[WEST].dy, false); break;
				}
			}

		}
	}
}

int Map::countNeighbors(int x, int y) const
{
	int c = 0;

	if (isWall(x - 1, y - 1))	c++; // NW
	if (isWall(x, y - 1))		c++; // N
	if (isWall(x + 1, y - 1))	c++; // NE
	if (isWall(x + 1, y))		c++; // E
	if (isWall(x + 1, y + 1))	c++; // SE
	if (isWall(x, y + 1))		c++; // S
	if (isWall(x - 1, y + 1))	c++; // SW
	if (isWall(x - 1, y))		c++; // W

	return c;
}


void Map::generateLife(const Rule& rule, float coef, int iter)
{
	fill(coef);

	for (int i = 0; i < iter; i++) {
		for (int x = 0; x < m_width; x++) {
			for (int y = 0; y < m_height; y++) {
				int c = countNeighbors(x, y);

				if (!isWall(x, y)) {
					// born
					bool born = false;

					for (int b = 0; b < (int)rule.B.size(); b++) {
						if (rule.B[b] == c) {
							born = true;
							break;
						}
					}

					setWall(x, y, born);
				} else {
					// stay
					bool stay = false;

					for (int s = 0; s < (int)rule.S.size(); s++) {
						if (rule.S[s] == c) {
							stay = true;
							break;
						}
					}

					setWall(x, y, stay);
				}

			}
		}
	}
}


void Map::fill(float coef)
{
	dig();

	for (int fill = 0; fill < ((float)(m_width * m_height) * coef); fill++) {
		setWall(Rnd::rndn() * (float)m_width, Rnd::rndn() * (float)m_height);
	}
}


void Map::generateRooms(int mw, int mh)
{
	fill();

	int ncol = (m_width - 1) / (mw + 1);
	int nrow = (m_height - 1) / (mh + 1);
	int x = 1;
	int y = 1;

	printf("ncol := %d\nnrow := %d\n", ncol, nrow);

	room *ul = new room[nrow * ncol];

	// create rooms
	for (int c = 0; c < ncol; c++) {
		for (int r = 0; r < nrow; r++) {
			room v;

			dig(x + c * (mw + 1),
				y + r * (mh + 1),
				x + (c + 1) * (mw + 1) - 2,
				y + (r + 1) * (mh + 1) - 2);

			// define north/south room linkages
			if (r == 0) {
				v.n[north] = none;
				v.n[south] = unlinked;
			} else if (r == (nrow - 1)) {
				v.n[north] = unlinked;
				v.n[south] = none;
			} else {
				v.n[north] = unlinked;
				v.n[south] = unlinked;
			}

			// define east/west room linkages
			if (c == 0) {
				v.n[west] = none;
				v.n[east] = unlinked;
			} else if (c == (ncol - 1)) {
				v.n[west] = unlinked;
				v.n[east] = none;
			} else {
				v.n[west] = unlinked;
				v.n[east] = unlinked;
			}

			v.visited = false;

			ul[c + r * ncol] = v;
		}
	}

	// links rooms
	// we want to randomly link adjacent rooms while ensuring that no two sets
	// of rooms are disjoint

	// we will accomplish this by a simple random walk
	int pivotcol = Rnd::between(0, ncol);
	int pivotrow = Rnd::between(0, nrow);
	int visited = 0;

	linkRooms(ncol, nrow, ul, &visited, pivotcol, pivotrow);
}


void Map::linkRooms(
	const int ncol, const int nrow,
	room* rooms, int *visited,
	int pc, int pr)
{
	int pivot = pc + pr * ncol;

	// have we visited this node yet?
	if (!rooms[pivot].visited) {
		rooms[pivot].visited = true;
		(*visited)++;
	}

	int x, y;
	static const int mh = 10;
	static const int mw = 10;

#if 0
	static const std::string dirNames[] = { "north", "south", "east ", "west " };
	static const char lnNames[] = { 'u', 'l', 'n' };

	Logger::log("pivot := %02d (%d, %d) [%c%c%c%c], vis := %d\n", pivot, pc, pr,
		lnNames[rooms[pivot].n[north]],
		lnNames[rooms[pivot].n[south]],
		lnNames[rooms[pivot].n[east]],
		lnNames[rooms[pivot].n[west]],
		*visited);
#endif

	while (*visited < (nrow * ncol)) {
		_dir dir = (_dir)Rnd::between(0, 4);

		// dead end? back up the stack
		if ((rooms[pivot].n[north] != unlinked) &&
			(rooms[pivot].n[south] != unlinked) &&
			(rooms[pivot].n[east] != unlinked) &&
			(rooms[pivot].n[west] != unlinked)) {
			return;
		}

		switch (dir) {
		case north:
			if (rooms[pivot].n[north] == unlinked) {
				rooms[pivot].n[north] = linked;

				// punch out wall
				y = pr * (mh + 1);
				x = Rnd::between(1 + pc * (mw + 1) + 1, (pc + 1) * (mw + 1) - 3);
				dig(x, y);

				// move north
				rooms[pc + (pr - 1) * ncol].n[south] = linked;
				linkRooms(ncol, nrow, rooms, visited, pc, pr - 1);
			}
			break;
		case south:
			if (rooms[pivot].n[south] == unlinked) {
				rooms[pivot].n[south] = linked;

				// punch out wall
				y = (pr + 1) * (mh + 1);
				x = Rnd::between(1 + pc * (mw + 1) + 1, (pc + 1) * (mw + 1) - 3);
				dig(x, y);

				// move south
				rooms[pc + (pr + 1) * ncol].n[north] = linked;
				linkRooms(ncol, nrow, rooms, visited, pc, pr + 1);
			}
			break;
		case east:
			if (rooms[pivot].n[east] == unlinked) {
				rooms[pivot].n[east] = linked;

				// punch out wall
				y = Rnd::between(1 + pr * (mh + 1) + 1, (pr + 1) * (mh + 1) - 3);
				x = (pc + 1) * (mw + 1);
				dig(x, y);

				// move east
				rooms[(pc + 1) + pr * ncol].n[west] = linked;
				linkRooms(ncol, nrow, rooms, visited, pc + 1, pr);
			}
			break;
		case west:
			if (rooms[pivot].n[west] == unlinked) {
				rooms[pivot].n[west] = linked;

				// punch out wall
				y = Rnd::between(1 + pr * (mh + 1) + 1, (pr + 1) * (mh + 1) - 3);
				x = pc * (mw + 1);
				dig(x, y);

				// move west
				rooms[(pc -1) + pr * ncol].n[east] = linked;
				linkRooms(ncol, nrow, rooms, visited, pc - 1, pr);
			}
			break;
		}
	}
}

void Map::placeRandomTorches(ObjectMap& dynamObj)
{
	static const int sMAX_TORCHES = 10;

	int nplaced = 0;

	while (nplaced < sMAX_TORCHES) {
		// pick a random point
		Point p(Rnd::rndn() * m_width, Rnd::rndn() * m_height);

		if (!isWall(p.x(), p.y())) {
			int i = p.x() + p.y() * m_width;

			delete dynamObj[i];
			dynamObj[i] = new Torch(p.x(), p.y(), 6, 10);

			MobilityModel *m = m_grid.list();
			m[i] = dynamObj[i]->mobilityModel();

			nplaced++;
		}
	}
}

void Map::dig(int x1, int y1, int x2, int y2)
{
	int lx = std::min(x1, x2);
	int mx = std::max(x1, x2);
	int ly = std::min(y1, y2);
	int my = std::max(y1, y2);

    for (int tilex = lx; tilex <= mx; tilex++) {
        for (int tiley = ly; tiley <= my; tiley++) {
			setWall(tilex, tiley, false);
        }
    }
}

void Map::dig(const Point& p1, const Point& p2)
{
	dig(p1.x(), p1.y(), p2.x(), p2.y());
}

void Map::dig(const Point& p)
{
	setWall(p.x(), p.y(), false);
}

void Map::dig(int x, int y)
{
	setWall(x, y, false);
}

void Map::dig(const Rect& r)
{
	dig(r.left(), r.top(), r.right(), r.bottom());
}

void Map::fill(int x1, int y1, int x2, int y2)
{
	int lx = std::min(x1, x2);
	int mx = std::max(x1, x2);
	int ly = std::min(y1, y2);
	int my = std::max(y1, y2);

    for (int tilex = lx; tilex <= mx; tilex++) {
        for (int tiley = ly; tiley <= my; tiley++) {
            setWall(tilex, tiley, true);
        }
    }
}

void Map::fill(const Point& p1, const Point& p2)
{
	fill(p1.x(), p1.y(), p2.x(), p2.y());
}

void Map::fill(const Point& p)
{
	setWall(p.x(), p.y(), true);
}

void Map::fill(int x, int y)
{
	setWall(x, y, true);
}

void Map::fill(const Rect& r)
{
	fill(r.left(), r.top(), r.right(), r.bottom());
}

void Map::fill()
{
	for (int x = 0; x < m_width; x++) {
		for (int y = 0; y < m_height; y++) {
			setWall(x, y);
		}
	}
}

void Map::dig()
{
	for (int x = 0; x < m_width; x++) {
		for (int y = 0; y < m_height; y++) {
			setWall(x, y, false);
		}
	}
}

const MobilityList* Map::staticCopy()
{
   return &m_grid;
}


Object* Map::staticObject(int x, int y)
{
	if (inbounds(x, y)) {
		return m_staticObjects[x + y * m_width];
	}
	return static_cast<Object*>(0);
}
