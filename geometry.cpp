#include "geometry.h"

#include <stdlib.h>
#include <math.h>
#include <algorithm>

///////////////////////////////////////////////////////////////////////////////
// Point																	 //
///////////////////////////////////////////////////////////////////////////////

const Point Point::ZERO(0, 0);

Point::Point() : m_x(0), m_y(0)
{
}

Point::Point(int x, int y) : m_x(x), m_y(y)
{
}

Point::Point(const Point& copy)
{
	operator=(copy);
}

Point::Point(const PointF& p) : m_x(p.x()), m_y(p.y())
{
}

Point::~Point()
{
}

Point& Point::operator=(const Point& rhs)
{
	if (this != &rhs) {
		m_x = rhs.m_x;
		m_y = rhs.m_y;
	}
	return *this;
}

float Point::distance() const
{
	return sqrt((float)compsqr());
}

bool Point::operator==(const Point& rhs) const
{
	return ((m_x == rhs.m_x) &&
			(m_y == rhs.m_y));
}

bool Point::operator!=(const Point& rhs) const
{
	return (false == operator==(rhs));
}

bool Point::operator<(const Point& rhs) const
{
//	return (((m_x < rhs.m_x) && (m_y < rhs.m_y)) ||
//			(compsqr() < rhs.compsqr()));
	return ((m_x < rhs.m_x) && (m_y < rhs.m_y));
}

bool Point::operator<=(const Point& rhs) const
{
	return ((true == operator<(rhs)) ||
			(true == operator==(rhs)));
}

bool Point::operator>(const Point& rhs) const
{
	return (false == operator<=(rhs));
}

bool Point::operator>=(const Point& rhs) const
{
	return (false == operator<(rhs));
}

Point& Point::operator+=(const Point& rhs)
{
	m_x += rhs.x();
	m_y += rhs.y();

	return *this;
}

Point& Point::operator-=(const Point& rhs)
{
	m_x -= rhs.x();
	m_y -= rhs.y();

	return *this;
}

bool Point::operator<(const PointF& rhs) const
{
	return operator<(Point(rhs.x(), rhs.y()));
}

bool Point::operator<=(const PointF& rhs) const
{
	return operator<=(Point(rhs.x(), rhs.y()));
}

bool Point::operator>(const PointF& rhs) const
{
	return operator>(Point(rhs.x(), rhs.y()));
}

bool Point::operator>=(const PointF& rhs) const
{
	return operator>=(Point(rhs.x(), rhs.y()));
}

Point& Point::operator=(const PointF& rhs)
{
	return operator=(Point(rhs.x(), rhs.y()));
}

bool Point::operator==(const PointF& rhs) const
{
	return operator==(Point(rhs.x(), rhs.y()));
}

bool Point::operator!=(const PointF& rhs) const
{
	return operator!=(Point(rhs.x(), rhs.y()));
}

Point& Point::operator+=(const PointF& rhs)
{
	return operator+=(Point(rhs.x(), rhs.y()));
}

Point& Point::operator-=(const PointF& rhs)
{
	return operator-=(Point(rhs.x(), rhs.y()));
}

Point& Point::operator*=(int rhs)
{
	m_x *= rhs;
	m_y *= rhs;

	return *this;
}

Point& Point::operator/=(int rhs)
{
	if (rhs != 0) {
		m_x /= rhs;
		m_y /= rhs;
	}

	return *this;
}

int Point::compsqr() const
{
	return SQR(m_x) + SQR(m_y);
}

int Point::manhatten(const Point& o) const
{
	return abs(m_x - o.x()) + abs(m_y - o.y());
}

int Point::chebyshev(const Point& o) const
{
	return std::max(abs(m_x - o.x()), abs(m_y - o.y()));
}

Point operator+(const Point& lhs, const Point& rhs)
{
	return Point(lhs.x() + rhs.x(), lhs.y() + rhs.y());
}

Point operator+(const Point& lhs, int		   rhs)
{
	return Point(lhs.x() + rhs, lhs.y() + rhs);
}

Point operator+(int			 lhs, const Point& rhs)
{
	return Point(lhs + rhs.x(), lhs + rhs.y());
}

Point operator-(const Point& lhs, const Point& rhs)
{
	return Point(lhs.x() - rhs.x(), lhs.y() - rhs.y());
}

Point operator-(const Point& lhs, int		   rhs)
{
	return Point(lhs.x() - rhs, lhs.y() - rhs);
}

Point operator-(int			 lhs, const Point& rhs)
{
	return Point(lhs - rhs.x(), lhs - rhs.y());
}

Point operator/(const Point& lhs, int		   rhs)
{
	return Point(lhs.x() / rhs, lhs.y() / rhs);
}

Point operator/(int			 lhs, const Point& rhs)
{
	return Point(lhs / rhs.x(), lhs / rhs.y());
}

Point operator*(const Point& lhs, int		   rhs)
{
	return Point(lhs.x() * rhs, lhs.y() * rhs);
}

Point operator*(int			 lhs, const Point& rhs)
{
	return Point(lhs * rhs.x(), lhs * rhs.y());
}

///////////////////////////////////////////////////////////////////////////////
// PointF																	 //
///////////////////////////////////////////////////////////////////////////////

PointF::PointF() : m_x(0), m_y(0)
{
}

PointF::PointF(float x, float y) : m_x(x), m_y(y)
{
}

PointF::PointF(const PointF& copy)
{
	operator=(copy);
}

PointF::PointF(const Point& p) : m_x(p.x()), m_y(p.y())
{
}

PointF::~PointF()
{
}

PointF& PointF::operator=(const PointF& rhs)
{
	if (this != &rhs) {
		m_x = rhs.m_x;
		m_y = rhs.m_y;
	}
	return *this;
}

float PointF::distance() const
{
	return sqrt((float)compsqr());
}

bool PointF::operator==(const PointF& rhs) const
{
	return ((m_x == rhs.m_x) &&
			(m_y == rhs.m_y));
}

bool PointF::operator!=(const PointF& rhs) const
{
	return (false == operator==(rhs));
}

bool PointF::operator<(const PointF& rhs) const
{
	return (((m_x < rhs.m_x) && (m_y < rhs.m_y)) ||
			(compsqr() < rhs.compsqr()));
}

bool PointF::operator<=(const PointF& rhs) const
{
	return ((true == operator<(rhs)) ||
			(true == operator==(rhs)));
}

bool PointF::operator>(const PointF& rhs) const
{
	return (false == operator<=(rhs));
}

bool PointF::operator>=(const PointF& rhs) const
{
	return (false == operator<(rhs));
}

PointF& PointF::operator+=(const PointF& rhs)
{
	m_x += rhs.x();
	m_y += rhs.y();

	return *this;
}

PointF& PointF::operator-=(const PointF& rhs)
{
	m_x -= rhs.x();
	m_y -= rhs.y();

	return *this;
}

bool PointF::operator<(const Point& rhs) const
{
	return operator<(PointF(rhs.x(), rhs.y()));
}

bool PointF::operator<=(const Point& rhs) const
{
	return operator<=(PointF(rhs.x(), rhs.y()));
}

bool PointF::operator>(const Point& rhs) const
{
	return operator>(PointF(rhs.x(), rhs.y()));
}

bool PointF::operator>=(const Point& rhs) const
{
	return operator>=(PointF(rhs.x(), rhs.y()));
}

PointF& PointF::operator+=(const Point& rhs)
{
	return operator+=(PointF(rhs.x(), rhs.y()));
}

PointF& PointF::operator-=(const Point& rhs)
{
	return operator-=(PointF(rhs.x(), rhs.y()));
}

PointF& PointF::operator=(const Point& rhs)
{
	return operator=(PointF(rhs.x(), rhs.y()));
}

bool PointF::operator==(const Point& rhs) const
{
	return operator==(PointF(rhs.x(), rhs.y()));
}

bool PointF::operator!=(const Point& rhs) const
{
	return operator!=(PointF(rhs.x(), rhs.y()));
}

PointF& PointF::operator*=(float rhs)
{
	m_x *= rhs;
	m_y *= rhs;

	return *this;
}

PointF& PointF::operator/=(float rhs)
{
	if (rhs != 0) {
		m_x /= rhs;
		m_y /= rhs;
	}

	return *this;
}

float PointF::compsqr() const
{
	return SQR(m_x) + SQR(m_y);
}

float PointF::manhatten() const
{
	return abs(m_x) + abs(m_y);
}


PointF operator+(const PointF& lhs, const PointF& rhs)
{
	return PointF(lhs.x() + rhs.x(), lhs.y() + rhs.y());
}

PointF operator+(const PointF& lhs, float		  rhs)
{
	return PointF(lhs.x() + rhs, lhs.y() + rhs);
}

PointF operator+(float		   lhs, const PointF& rhs)
{
	return PointF(lhs + rhs.x(), lhs + rhs.y());
}

PointF operator-(const PointF& lhs, const PointF& rhs)
{
	return PointF(lhs.x() - rhs.x(), lhs.y() - rhs.y());
}

PointF operator-(const PointF& lhs, float		  rhs)
{
	return PointF(lhs.x() - rhs, lhs.y() - rhs);
}

PointF operator-(float		   lhs, const PointF& rhs)
{
	return PointF(lhs - rhs.x(), lhs - rhs.y());
}

PointF operator/(const PointF& lhs, float		  rhs)
{
	return PointF(lhs.x() / rhs, lhs.y() / rhs);
}

PointF operator/(float		   lhs, const PointF& rhs)
{
	return PointF(lhs / rhs.x(), lhs / rhs.y());
}

PointF operator*(const PointF& lhs, float		  rhs)
{
	return PointF(lhs.x() * rhs, lhs.y() * rhs);
}

PointF operator*(float		   lhs, const PointF& rhs)
{
	return PointF(lhs * rhs.x(), lhs * rhs.y());
}

///////////////////////////////////////////////////////////////////////////////
// Size																		 //
///////////////////////////////////////////////////////////////////////////////

Size::Size() : m_width(0), m_height(0)
{
}

Size::Size(int w, int h) : m_width(w), m_height(h)
{
}

Size::Size(const Size& copy)
{
	operator=(copy);
}

Size::~Size()
{
}

Size& Size::operator=(const Size& rhs)
{
	if (this != &rhs) {
		m_width  = rhs.m_width;
		m_height = rhs.m_height;
	}

	return *this;
}

bool Size::operator==(const Size& rhs) const
{
	return ((m_width  == rhs.m_width) &&
			(m_height == rhs.m_height));
}

bool Size::operator!=(const Size& rhs) const
{
	return (false == operator==(rhs));
}

Size& Size::operator+=(const Size& rhs)
{
	if (this != &rhs) {
		m_width  += rhs.m_width;
		m_height += rhs.m_height;
	}

	return *this;
}

Size& Size::operator-=(const Size& rhs)
{
	if (this != &rhs) {
		m_width  -= rhs.m_width;
		m_height -= rhs.m_height;
	}

	return *this;
}

Size& Size::operator*=(int rhs)
{
	m_width  *= rhs;
	m_height *= rhs;

	return *this;
}

Size& Size::operator/=(int rhs)
{
	if (rhs != 0) {
		m_width  /= rhs;
		m_height /= rhs;
	}

	return *this;
}

bool Size::inbounds(int x, int y) const
{
	return ((x >= 0) && (x < m_width) &&
			(y >= 0) && (y < m_height));
}

Size operator+(const Size& lhs, const Size& rhs)
{
	return Size(lhs.width() + rhs.width(),
				lhs.height() + rhs.height());
}

Size operator+(const Size& lhs, int		    rhs)
{
	return Size(lhs.width() + rhs,
				lhs.height() + rhs);
}

Size operator+(int		   lhs, const Size& rhs)
{
	return Size(lhs + rhs.width(),
				lhs + rhs.height());
}

Size operator-(const Size& lhs, const Size& rhs)
{
	return Size(lhs.width() - rhs.width(),
				lhs.height() - rhs.height());
}

Size operator-(const Size& lhs, int		    rhs)
{
	return Size(lhs.width() - rhs,
				lhs.height() - rhs);
}

Size operator-(int		   lhs, const Size& rhs)
{
	return Size(lhs - rhs.width(),
				lhs - rhs.height());
}

Size operator/(const Size& lhs, int		    rhs)
{
	return Size(lhs.width() / rhs,
				lhs.height() / rhs);
}

Size operator/(int		   lhs, const Size& rhs)	
{
	return Size(lhs / rhs.width(),
				lhs / rhs.height());
}

Size operator*(const Size& lhs, int		    rhs)
{
	return Size(lhs.width() * rhs,
				lhs.height() * rhs);
}

Size operator*(int		   lhs, const Size& rhs)
{
	return Size(lhs * rhs.width(),
				lhs * rhs.height());
}

///////////////////////////////////////////////////////////////////////////////
// Rect																		 //
///////////////////////////////////////////////////////////////////////////////

Rect::Rect() :
	m_top(0), m_left(0), m_bottom(0), m_right(0)
{
}

Rect::Rect(int top, int left, int bottom, int right)
{
	construct(top, left, bottom, right);
}

Rect::Rect(int top, int left, const Size& s) :
	m_top(top), m_left(left),
	m_bottom(top + s.height()), m_right(left + s.width())
{
}

Rect::Rect(const Point& tl, const Point& br)
{
	construct(tl.y(), tl.x(), br.y(), br.x());
}

Rect::Rect(const Rect& copy)
{
	if (this != &copy) {
		m_top		= copy.m_top;
		m_bottom	= copy.m_bottom;
		m_left		= copy.m_left;
		m_right		= copy.m_right;
	}
}

Rect::~Rect()
{
}

void Rect::construct(int t, int l, int b, int r)
{
	m_top = std::min(t, b);
	m_bottom = std::max(t, b);
	m_left = std::min(l, r);
	m_right = std::max(l, r);
}

Point Rect::topLeft() const
{
	return Point(m_left, m_top);
}

Point Rect::bottomRight() const
{
	return Point(m_right, m_bottom);
}

Rect& Rect::operator=(const Rect& rhs)
{
	if (this != &rhs) {
		m_top		= rhs.m_top;
		m_bottom	= rhs.m_bottom;
		m_left		= rhs.m_left;
		m_right		= rhs.m_right;
	}

	return *this;
}

bool Rect::operator==(const Rect& rhs) const
{
	return ((m_left == rhs.m_left) &&
			(m_right == rhs.m_right) &&
			(m_top == rhs.m_top) &&
			(m_bottom == rhs.m_bottom));
}

bool Rect::operator!=(const Rect& rhs) const
{
	return (false == operator==(rhs));
}


Rect& Rect::operator+=(const Size& rhs)
{
	m_right += rhs.width();
	m_bottom += rhs.height();

	return *this;
}
Rect& Rect::operator-=(const Size& rhs)
{
	m_right -= rhs.width();
	m_bottom -= rhs.height();

	return *this;
}

bool Rect::inbounds(int x, int y) const
{
	return (((x >= m_left) && (x < m_right)) &&
			((y >= m_top) && (y < m_bottom)));
}

bool Rect::inbounds(const Point& p) const
{
	return inbounds(p.x(), p.y());
}

bool Rect::inbounds(const Rect& r) const
{
	return ((inbounds(r.topLeft())) &&
			(inbounds(r.bottomRight() - Point(1, 1))));
}

int Rect::area() const
{
	int w = m_right - m_left;
	int h = m_bottom - m_top;

	return w * h;
}

void Rect::move(int dx, int dy)
{
	m_left		+= dx;
	m_right		+= dx;
	m_top		+= dy;
	m_bottom	+= dy;
}

Rect Rect::expand(int s) const
{
	if ((2 * s) > width()) return Rect();

	return Rect(m_top - s, m_left - s, m_bottom + s, m_right + s);
}

Rect Rect::expand(const Size& s) const
{
	if ((2 * s.width()) > width()) return Rect();
	if ((2 * s.height()) > height()) return Rect();

	return Rect(m_top - s.height(), m_left - s.width(),
				m_bottom + s.height(), m_right + s.width());
}

Rect Rect::shrink(int s) const
{
	if ((2 * s) > width()) return Rect();

	return Rect(m_top + s, m_left + s, m_bottom - s, m_right - s);
}

Rect Rect::shrink(const Size& s) const
{
	if ((2 * s.width()) > width()) return Rect();
	if ((2 * s.height()) > height()) return Rect();

	return Rect(m_top + s.height(), m_left + s.width(),
				m_bottom - s.height(), m_right - s.width());
}
