#include "pathfinding.h"
#include "viewport.h"

#include <math.h>
#include <set>
#include <unordered_map>
#include <map>
#include <queue>

///////////////////////////////////////////////////////////////////////////////

Pathfinder::Pathfinder()
{
}

Pathfinder::~Pathfinder()
{
}

void Pathfinder::dijkstra(PVector& path, TheGrid *grid, const Point& s, const Point& g)
{
	PriorityQueue frontier;
	PriorityQueue::ParentMap parents;
	PriorityQueue::CostMap costs;
//	std::map<PriorityQueue::Location, PriorityQueue::Location> parents;
//	std::map<PriorityQueue::Location, float> costs;

	PriorityQueue::Location cur;
	PriorityQueue::Location n = { s.x(), s.y() };

	static const float fake_costs[NNEIGHBORS] = {
		1.41421356237f,	// NORTHEAST
		1,				// EAST
		1.41421356237f,	// SOUTHEAST
		1,				// NORTH
		1,				// SOUTH
		1.41421356237f,	// NORTHWEST
		1,				// WEST
		1.41421356237f,	// SOUTHWEST
	};

	path.clear();

	costs[n] = g.chebyshev(Point(n.x, n.y));
	parents[n] = n;

	frontier.put(s, 0.0f);

	// end tile not walkable, return
	if (!(grid->get(g)->render->mobility.flags & M_WALKABLE)) return;

	while (!frontier.empty()) {
		cur = frontier.get();

//		console->setCharBackground(cur.x, cur.y, TCODColor::blue);

		if (Point(cur.x, cur.y) == g) {
			break;
		}

		for (int i = 0; i < NNEIGHBORS; i++) {
			n.x = cur.x + NEIGHBORS[i].dx;
			n.y = cur.y + NEIGHBORS[i].dy;

			float cost = costs[cur] + fake_costs[i];

			if ((grid->inbounds(Point(n.x, n.y))) && (grid->get(g)->render->mobility.flags & M_WALKABLE)) {
				bool visited = (costs.count(n) > 0);

				if ((!visited) || (cost < costs[n])) {
					costs[n] = cost;
					parents[n] = cur;

					// adding the chebyshev (or manhatten) distances uses a heurisitc and
					// thus this algorithm becomes A*.

					// as we move farther away, make our algorithm greedier
					int cheb = g.chebyshev(Point(n.x, n.y));
					static const int thresh = std::max(12, g.chebyshev(s) / 8);

#ifdef DONT_REVISIT
					if (!visited) {
#else
					{
#endif
						if (cheb > thresh) {
							// using an inadmissible heuristic should improve performance
							frontier.put(n, cost + g.manhatten(Point(n.x, n.y)));
						} else {
							frontier.put(n, cost + cheb);
						}
					}

//					console->setCharBackground(n.x, n.y, TCODColor::red);
				}
			}
		}
	}

	PriorityQueue::Location c = { g.x(), g.y() };
	path.push_back(Point(c.x, c.y));

	while (Point(c.x, c.y) != s) {
		c = parents[c];
		path.push_back(Point(c.x, c.y));
//		console->setCharBackground(c.x, c.y, TCODColor::lighterYellow);
	}
}

///////////////////////////////////////////////////////////////////////////////
// This code is based off code by Joshua Day (https://github.com/joshuaday)
// and his progressive dijkstra scan

void PDSQueue::clear()
{
	m_front.next = NULL;

	for (int i = 0; i < WINDOW_WIDTH * WINDOW_HEIGHT; i++) {
		m_list[i].distance = PDS_MAX_DISTANCE;
		m_list[i].prev = NULL;
		m_list[i].next = NULL;
	}
}

void PDSQueue::update()
{
	short dir;
	PDSNode *prev = NULL, *next = NULL, *link = NULL;

	PDSNode *head = m_front.next;
	m_front.next = NULL;

	while (head) {
		for (dir = 0; dir < NNEIGHBORS; dir++) {
			link = head + (NEIGHBORS[dir].dx + WINDOW_WIDTH * NEIGHBORS[dir].dy);

			// off map?
			if ((link < m_list) || (link >= (m_list + WINDOW_WIDTH * WINDOW_HEIGHT))) continue;

			// obstructed
			if (link->cost < 0) continue;

			// only diagonal
			if (!cardinal(dir)) {
				PDSNode *w1, *w2;
				w1 = head + NEIGHBORS[dir].dx;
				w2 = head + WINDOW_WIDTH * NEIGHBORS[dir].dy;

				if ((w1->cost == PDS_OBSTRUCTION) || (w2->cost == PDS_OBSTRUCTION)) continue;
			}

			if ((head->distance + link->cost) < link->distance) {
				link->distance = head->distance + link->cost;

				if (link->next) { link->next->prev = link->prev; }
				if (link->prev) { link->prev->next = link->next; }

				prev = head;
				next = head->next;

				while ((next) && (next->distance < link->distance)) {
					prev = next;
					next = next->next;
				}

				if (prev) { prev->next = link; }
				link->next = next;
				link->prev = prev;
				if (next) { next->prev = link; }
			}
		}

		next = head->next;
		
		head->prev = NULL;
		head->next = NULL;

		head = next;
	}
}

void PDSQueue::input(WeightMap* distance, WeightMap* cost)
{
	PDSNode *prev = NULL, *next = NULL, *link = NULL;

	m_front.next = NULL;

	if ((!distance) || (!cost)) return;

	for (int x = 0; x < WINDOW_WIDTH; x++) {
		for (int y = 0; y < WINDOW_HEIGHT; y++) {
			link = m_list + (x + WINDOW_WIDTH * y);

			int c = *(cost->at(x, y));

			link->cost = c;
			link->distance = *(distance->at(x, y));

			if (c > 0) {
				if (link->distance < PDS_MAX_DISTANCE) {
					if ((next == NULL) || (next->distance > link->distance)) {
						prev = &m_front;
						next = m_front.next;
					}

					while ((next) && (next->distance < link->distance)) {
						prev = next;
						next = next->next;
					}

					link->next = next;
					link->prev = prev;
					prev->next = link;

					if (next) { next->prev = link; }

					prev = link;
				} else {
					link->next = NULL;
					link->prev = NULL;
				}
			} else {
				link->next = NULL;
				link->prev = NULL;
			}
		}
	}
}


void PDSQueue::scan(WeightMap* distance, WeightMap* cost)
{
//	clear();
	input(distance, cost);
	update();
	m_farthest = 0;

	for (int x = 0; x < WINDOW_WIDTH; x++) {
		for (int y = 0; y < WINDOW_HEIGHT; y++) {
			short d = (m_list + (x + y * WINDOW_WIDTH))->distance;
			*(distance->at(x, y)) = d;
			if (d > m_farthest) { m_farthest = d; }
		}
	}
}
