#pragma once

#include "raylib.h"

#include <stdarg.h>
#include <set>

namespace gtti {

class Color
{
public:
    Color();
    Color(int r, int g, int b, unsigned char a = 255);
    Color(const ::Color& tc);
    Color(const Color& copy);
    Color(unsigned int rgba);	// rgba

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

    inline int r() const { return m_red; }
    inline int g() const { return m_green; }
    inline int b() const { return m_blue; }

    inline unsigned char alpha() const { return m_alpha; }

    unsigned int packed() const;

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

    // alpha blend the given color with the base color using the given alpha percent
    void blend(const Color &o, float percent);
    static Color blend(const Color& base, const Color &o, float percent);

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

    inline ::Color toColor() const
    {
        Color c = Color::normalize(*this);
        
        ::Color ret;
        ret.a = c.alpha();
        ret.r = c.r();
        ret.g = c.g();
        ret.b = c.b();

        return ret;
    }

    float hsvValue() const;
    float hsvSaturation() const;
    float hsvHue() const;

    void setHSV(float hue, float saturation, float value);
    void scaleHSV(float satCoef = 1.0f, float valueCoef = 1.0f);

public:
    static const Color blank;

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
    unsigned char m_alpha;
};

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

// Color gradient class
class Gradient
{
public:
    // lerp(a, b)
    Gradient(const Color& a = Color::black, const Color& b = Color::black);
    // 0.0-0.5: lerp(a, b)
    // 0.5-1.0: lerp(b, c)
    Gradient(const Color& a, const Color& b, const Color& c);
    Gradient(int ncolors, ...);
    ~Gradient();

    // You can only add colors to the middle (between 0.0 and 1.0)
    void addColor(const Color& c, float v);

    // returns the color along the gradient, r is an element of [0,1]
    Color getColor(float r) const;

    Gradient& operator=(const Gradient& rhs);

protected:

    struct GradientPoint
    {
        float m_value;
        Color m_color;

        bool operator<(const GradientPoint& rhs) const
        {
            return (m_value < rhs.m_value);
        }

        bool operator==(const GradientPoint& rhs) const
        {
            return (m_value == rhs.m_value);
        }

        GradientPoint& operator=(const GradientPoint& rhs)
        {
            if (this != &rhs) {
                m_value = rhs.m_value;
                m_color = rhs.m_color;
            }

            return *this;
        }
    };

    std::set<GradientPoint> m_points;
};

} //namespace gtti