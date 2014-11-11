#pragma once

#include <vector>

#define SQR(x) ((x) * (x))

struct DN {
	int dx;
	int dy;
};

#define NNEIGHBORS	8

static const DN NEIGHBORS[NNEIGHBORS + 1] = {
	{  1,  0 },	// EAST
	{  1,  1 },	// SOUTHEAST
	{  0,  1 },	// SOUTH
	{ -1,  1 }, // SOUTHWEST
	{ -1,  0 }, // WEST
	{ -1, -1 },	// NORTHWEST
	{  0, -1 },	// NORTH
	{  1, -1 },	// NORTHEAST
	{  0,  0 }
};

#define EAST		0
#define SOUTHEAST	1
#define SOUTH		2
#define SOUTHWEST	3
#define WEST		4
#define NORTHWEST	5
#define NORTH		6
#define NORTHEAST	7

#define NCARDINAL	4

static const DN CARDINAL[NCARDINAL] = {
	NEIGHBORS[NORTH],
	NEIGHBORS[SOUTH],
	NEIGHBORS[EAST],
	NEIGHBORS[WEST]
};

static bool cardinal(int dir)
{
	return ((dir == EAST) ||
			(dir == NORTH) ||
			(dir == SOUTH) ||
			(dir == WEST));
}

///////////////////////////////////////////////////////////////////////////////
// Point																	 //
///////////////////////////////////////////////////////////////////////////////

class PointF;

class Point
{
public:
	Point();
	Point(int x, int y);
	Point(const Point& copy);
	Point(const PointF& p);
	~Point();

	int x() const;
	int y() const;

	static const Point ZERO;

	// operations on a const Point& 
	bool operator<(const Point& rhs) const;
	bool operator<=(const Point& rhs) const;
	bool operator>(const Point& rhs) const;
	bool operator>=(const Point& rhs) const;

	Point& operator=(const Point& rhs);
	bool operator==(const Point& rhs) const;
	bool operator!=(const Point& rhs) const;

	Point& operator+=(const Point& rhs);
	Point& operator-=(const Point& rhs);

	// operations on a const PointF& 
	bool operator<(const PointF& rhs) const;
	bool operator<=(const PointF& rhs) const;
	bool operator>(const PointF& rhs) const;
	bool operator>=(const PointF& rhs) const;

	Point& operator=(const PointF& rhs);
	bool operator==(const PointF& rhs) const;
	bool operator!=(const PointF& rhs) const;

	Point& operator+=(const PointF& rhs);
	Point& operator-=(const PointF& rhs);

	// operations on a float
	Point& operator*=(int rhs);
	Point& operator/=(int rhs);

	int manhatten(const Point& o = ZERO) const;
	int chebyshev(const Point& o = ZERO) const;

	float distance() const;

protected:

	int compsqr() const;

	int m_x;
	int m_y;
};

inline
int Point::x() const
{
	return m_x;
}

inline
int Point::y() const
{
	return m_y;
}

Point operator+(const Point& lhs, const Point& rhs);
Point operator+(const Point& lhs, int		   rhs);
Point operator+(int			 lhs, const Point& rhs);

Point operator-(const Point& lhs, const Point& rhs);
Point operator-(const Point& lhs, int		   rhs);
Point operator-(int			 lhs, const Point& rhs);

Point operator/(const Point& lhs, int		   rhs);
Point operator/(int			 lhs, const Point& rhs);
Point operator*(const Point& lhs, int		   rhs);
Point operator*(int			 lhs, const Point& rhs);

typedef Point* PointP;
typedef std::vector<Point> PVector;

///////////////////////////////////////////////////////////////////////////////
// PointF																	 //
///////////////////////////////////////////////////////////////////////////////

class PointF
{
public:
	PointF();
	PointF(float x, float y);
	PointF(const PointF& copy);
	PointF(const Point& p);
	~PointF();

	float x() const;
	float y() const;

	// operations on a const PointF& 
	bool operator<(const PointF& rhs) const;
	bool operator<=(const PointF& rhs) const;
	bool operator>(const PointF& rhs) const;
	bool operator>=(const PointF& rhs) const;

	PointF& operator+=(const PointF& rhs);
	PointF& operator-=(const PointF& rhs);

	PointF& operator=(const PointF& rhs);
	bool operator==(const PointF& rhs) const;
	bool operator!=(const PointF& rhs) const;

	// operations on a const Point& 
	bool operator<(const Point& rhs) const;
	bool operator<=(const Point& rhs) const;
	bool operator>(const Point& rhs) const;
	bool operator>=(const Point& rhs) const;

	PointF& operator+=(const Point& rhs);
	PointF& operator-=(const Point& rhs);

	PointF& operator=(const Point& rhs);
	bool operator==(const Point& rhs) const;
	bool operator!=(const Point& rhs) const;

	// operations on a float
	PointF& operator*=(float rhs);
	PointF& operator/=(float rhs);

	float manhatten() const;
	float distance() const;

protected:

	float compsqr() const;

	float m_x;
	float m_y;
};

inline
float PointF::x() const
{
	return m_x;
}

inline
float PointF::y() const
{
	return m_y;
}

PointF operator+(const PointF& lhs, const PointF& rhs);
PointF operator+(const PointF& lhs, float		  rhs);
PointF operator+(float		   lhs, const PointF& rhs);

PointF operator-(const PointF& lhs, const PointF& rhs);
PointF operator-(const PointF& lhs, float		  rhs);
PointF operator-(float		   lhs, const PointF& rhs);

PointF operator/(const PointF& lhs, float		  rhs);
PointF operator/(float		   lhs, const PointF& rhs);
PointF operator*(const PointF& lhs, float		  rhs);
PointF operator*(float		   lhs, const PointF& rhs);

typedef PointF* PointFP;
typedef std::vector<PointF> PFVector;

///////////////////////////////////////////////////////////////////////////////
// Size																		 //
///////////////////////////////////////////////////////////////////////////////

class Size
{
public:
	Size();
	Size(int w, int h);
	Size(const Size& copy);
	~Size();

	int width() const;
	int height() const;

	int area() const;

	Size& operator=(const Size& rhs);
	bool operator==(const Size& rhs) const;
	bool operator!=(const Size& rhs) const;

	Size& operator+=(const Size& rhs);
	Size& operator-=(const Size& rhs);
	Size& operator*=(int rhs);
	Size& operator/=(int rhs);

	bool inbounds(int x, int y) const;

protected:

	int m_width;
	int m_height;
};

inline
int Size::width() const
{
	return m_width;
}

inline
int Size::height() const
{
	return m_height;
}

inline
int Size::area() const
{
	return m_width * m_height;
}

Size operator+(const Size& lhs, const Size& rhs);
Size operator+(const Size& lhs, int		    rhs);
Size operator+(int		   lhs, const Size& rhs);

Size operator-(const Size& lhs, const Size& rhs);
Size operator-(const Size& lhs, int		    rhs);
Size operator-(int		   lhs, const Size& rhs);

Size operator/(const Size& lhs, int		    rhs);
Size operator/(int		   lhs, const Size& rhs);
Size operator*(const Size& lhs, int		    rhs);
Size operator*(int		   lhs, const Size& rhs);

///////////////////////////////////////////////////////////////////////////////
// Rect																		 //
///////////////////////////////////////////////////////////////////////////////

class Rect
{
public:
	Rect();
	Rect(int top, int left, int bottom, int right);
	Rect(int top, int left, const Size& s);
	Rect(const Point& tl, const Point& br);
	Rect(const Rect& copy);

	~Rect();

	Point topLeft() const;
	Point bottomRight() const;

	int top() const;
	int left() const;
	int bottom() const;
	int right() const;

	Rect& operator=(const Rect& rhs);
	bool operator==(const Rect& rhs) const;
	bool operator!=(const Rect& rhs) const;

	// add or subtract width/height to the rect
	Rect& operator+=(const Size& rhs);
	Rect& operator-=(const Size& rhs);

	bool inbounds(int x, int y) const;
	bool inbounds(const Point& p) const;

	// checks if r is completely inscribed in this
	bool inbounds(const Rect& r) const;

	void move(int dx, int dy);

	int area() const;
	int width() const;
	int height() const;

	Size size() const;

	Rect expand(int s) const;
	Rect expand(const Size& s) const;
	Rect shrink(int s) const;
	Rect shrink(const Size& s) const;

protected:

	void construct(int t, int l, int b, int r);

protected:
	int m_top;
	int m_left;
	int m_bottom;
	int m_right;
};

inline
int Rect::top() const
{
	return m_top;
}

inline
int Rect::left() const
{
	return m_left;
}

inline
int Rect::bottom() const
{
	return m_bottom;
}

inline
int Rect::right() const
{
	return m_right;
}

inline
int Rect::width() const
{
	return m_right - m_left;
}

inline
int Rect::height() const
{
	return m_bottom - m_top;
}

inline
Size Rect::size() const
{
	return Size(m_right - m_left, m_bottom - m_top);
}
