#pragma once

#include "geometry.h"

#define WINDOW_WIDTH	96
#define WINDOW_HEIGHT	72

class Viewport
{
public:
	Viewport(const Size& container,
			 const Size& viewport,
			 int renderSpace,
			 int x = 0, int y = 0);

	// attempts the center the viewport on (x, y)
	void lookat(int x, int y);

	// scrolls the viewport directionally
	void scroll(int dx, int dy);

	Rect viewport() const;

	// returns a clamped render
	Rect render() const;

	Point screenToMap(const Point& p) const;

protected:
	
	// the total size of the viewport
	Size m_container;

	// this variable defines the distance from the edge of the viewport that
	// we want to try to maintain with the player
	Size m_offset;

	// this rect defines what part of the container the viewport is currently
	// viewing
	Rect m_viewport;

	// this rect defines what part of the container should be considered for
	// rendering and updating
	Rect m_renderport;

	// the render offset
	int m_renderSpace;

	Point m_target;
};

inline
Rect Viewport::viewport() const
{
	return m_viewport;
}
