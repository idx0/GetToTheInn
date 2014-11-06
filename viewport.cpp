#include "viewport.h"
#include "util.h"

#include <algorithm>

Viewport::Viewport(const Size& container,
				   const Size& viewport,
				   int renderSpace, int x, int y) :
	m_container(container),
	m_offset(25, 25),
	m_viewport(y, x, viewport),
	m_renderSpace(renderSpace)
{
	lookat(x, y);
}

void Viewport::lookat(int x, int y)
{
	// min/max in the x directions for the left of the viewport
	const int mnx = 0;
	const int mxx = m_container.width() - m_viewport.width();

	// min/max in the y directions for the top of the viewport
	const int mny = 0;
	const int mxy = m_container.height() - m_viewport.height();

	int t = y - m_viewport.height() / 2;
	int l = x - m_viewport.width() / 2;

	Utils::clamp(t, mny, mxy);
	Utils::clamp(l, mnx, mxx);

	m_target = Point(x, y);
	m_viewport = Rect(t, l, m_viewport.size());
	m_renderport = m_viewport.expand(m_renderSpace);
}

void Viewport::scroll(int dx, int dy)
{
	// min/max in the x directions for the left of the viewport
	const int mnx = 0;
	const int mxx = m_container.width() - m_viewport.width();

	// min/max in the y directions for the top of the viewport
	const int mny = 0;
	const int mxy = m_container.height() - m_viewport.height();

	int t = m_viewport.top(), l = m_viewport.left();
	int x = m_target.x(), y = m_target.y();

	// left/right
	if (dx < 0) {
		x += dx;
		if (x < (l + m_offset.width())) {
			l += dx;
		}
	} else if (dx > 0) {
		x += dx;
		if (x > (l + (m_viewport.width() - m_offset.width()))) {
			l += dx;
		}
	}

	// up/down
	if (dy < 0) {
		y += dy;
		if (y < (t + m_offset.height())) {
			t += dy;
		}
	} else if (dy > 0) {
		y += dy;
		if (y > (t + (m_viewport.height() - m_offset.height()))) {
			t += dy;
		}
	}

	Utils::clamp(t, mny, mxy);
	Utils::clamp(l, mnx, mxx);

	m_target = Point(x, y);
	m_viewport = Rect(t, l, m_viewport.size());
	m_renderport = m_viewport.expand(m_renderSpace);
}

Rect Viewport::render() const
{
	int t = std::max(0, m_renderport.top());
	int l = std::max(0, m_renderport.left());
	int b = std::min(m_container.height(), m_renderport.bottom());
	int r = std::min(m_container.width(), m_renderport.right());

	return Rect(t, l, b, r);
}


Point Viewport::screenToMap(const Point& p) const
{
	return Point(p.x() + m_viewport.left(), p.y() + m_viewport.top());
}