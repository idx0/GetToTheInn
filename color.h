#pragma once

#include <libtcod.hpp>

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

	enum GreyscaleType
	{
		C_GREY_LIGHTNESS,
		C_GREY_LUMINOSITY,
		C_GREY_AVERAGE
	};

	// returns the average of all colors
	float average() const;

	// returns a greyscale of the given color
	static Color greyscale(const Color& c, const GreyscaleType& type = C_GREY_LUMINOSITY);
	void greyscale(const GreyscaleType& type = C_GREY_LUMINOSITY);

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
