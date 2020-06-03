#ifndef __INCLUDE_TILE_ENGINE_H__
#define __INCLUDE_TILE_ENGINE_H__

#include "raylib.h"
#include "geometry.h"
#include "color.h"
#include "rnd.h"
#include "delay.h"

#include <string>
#include <set>

class TileEngine;

namespace gtti {

class texture
{
    Texture2D _texture;
    bool _valid = false;
public:
    texture() = default;
    texture(const texture&) = default;
    texture(texture&&) = default;

    texture(const char *filename) : _valid(true)
    {
        _texture = LoadTexture(filename);
    }

    texture(Image img) : _valid(true)
    {
        _texture = LoadTextureFromImage(img);
    }

    ~texture()
    {
        unload();
    }

    texture& operator=(const texture&) = default;
    texture& operator=(texture&&) = default;

public:
    operator Texture2D() const { return _texture; }
    operator bool() const { return _valid; }

    void load(const char *filename)
    {
        unload();
        _texture = LoadTexture(filename);
        _valid = true;
    }

    void load(Image img)
    {
        unload();
        _texture = LoadTextureFromImage(img);
        _valid = true;
    }

    void unload()
    {
        if (_valid) {
            UnloadTexture(_texture);
        }
        _valid = false;
    }
};

} // namespace gtti

struct Icon
{
    unsigned char _val = 0;
    Color _fg = WHITE;
    Color _bg = BLANK;
};

class Filter
{
public:
    virtual ~Filter() {}
    virtual Icon operator()(const Icon &, int, int) = 0;
};

class NoFilter : public Filter
{
public:
    inline Icon operator()(const Icon &i, int, int) { return i; }
};

class PsychedlicFilter : public Filter
{
    float alphaPhase = 0.78539816339f; // pi/4
    float blendPhase = 0.0f;
    float blendFreq = 1.0f;
    float alphaFreq = 2.5f;
    ConstantDelay changeTick;

    float tick = 0.0f;

public:
    PsychedlicFilter();
    virtual ~PsychedlicFilter();

    Icon operator()(const Icon &i, int x, int y);
};

class FadeFilter : public Filter
{
    enum State
    {
        FF_IDLE,
        FF_OUT,
        FF_HOLD,
        FF_IN
    };

    ConstantDelay _in, _out, _hold;
    State _state = FF_OUT;
    float _alpha = 1.0f;
    gtti::Color _c;

public:
    // out -> s to fade to c
    // hold -> s to hold at c
    // in -> s to fade from c
    FadeFilter(float out, float hold, float in, const gtti::Color &c);
    virtual ~FadeFilter();

    bool done() const { return _state == FF_IDLE; }
    void reset() { _state = FF_OUT; }

    Icon operator()(const Icon &i, int x, int y);
};

class ThunderstormFilter : public Filter
{
    FadeFilter _lightening;
    ConstantDelay _rain;
    bool _isRaining = true;

    ConstantDelay _rate;
    unsigned int _count = 0;
    unsigned int _tick = 0;

public:
    ThunderstormFilter();
    virtual ~ThunderstormFilter();

    Icon operator()(const Icon &i, int x, int y);
};

class Console
{
    friend class TileEngine;

    class Tile
    {
        Icon _icon;
        Size _size;

    public:
        Tile() {}
        Tile(unsigned char v) { _icon._val = 0;  }

        inline void setChar(unsigned char v) { _icon._val = v; }
        inline void setForegroundColor(Color fg) { _icon._fg = fg; }
        inline void setBackgroundColor(Color bg) { _icon._bg = bg; }

        Tile(const Tile&) = default;
        Tile(Tile&&) = default;

        Tile& operator=(const Tile&) = default;
        Tile& operator=(Tile&&) = default;

        void setDimensions(int w, int h);
        void draw(Filter *filter, Texture2D tex, int x, int y) const;

        Color getForegroundColor() const { return _icon._fg; }
        Color getBackgroundColor() const { return _icon._bg; }
        unsigned char getChar() const { return _icon._val; }
    };

    Console::Tile *_table;
    gtti::texture _texture;
    std::string _font;
    Filter *_filter = nullptr;
    bool _visible = true;
    gtti::Color _bg = gtti::Color::blank;

    Point _pos;
    Size _csize;
    Size _size;

    Console() = delete;
    Console(const Console&) = delete;
    Console(Console&&) = delete;

    Console(int w, int h);
    Console(const Console &c, int w, int h);

public:
    Console(const char *font, int cw, int ch, int sw, int sh);
    ~Console();

    inline Tile& at(int x, int y) { return _table[x + y * _size.width()]; }
    inline const Tile& at(int x, int y) const { return _table[x + y * _size.width()]; }

    inline int width() const { return _size.width(); }
    inline int height() const { return _size.height(); }

    Point position() const { _pos; }
    void setPosition(const Point &p) { _pos = p; }

    inline void setChar(int x, int y, unsigned char v) { at(x, y).setChar(v); }
    inline void setCharForeground(int x, int y, Color fg) { at(x, y).setForegroundColor(fg); }
    inline void setCharBackground(int x, int y, Color bg) { at(x, y).setBackgroundColor(bg); }

    inline void setBackgroundColor(const gtti::Color &c) { _bg = c;  }

    void getCharSize(int *w, int *h);
    void apply(Console *other, int x, int y);
    void blend(Console *other, int x, int y, float fgBlend = 1.0f, float bgBlend = 1.0f);

    void setFilter(Filter *f);

    inline void hide() { _visible = false; }
    inline void show() { _visible = true; }

    void clear();
    void draw();
};

class TileEngine
{
    struct ConsoleLayer
    {
        unsigned int id;
        std::string name;
        Console *console;
        Rect pos;

        bool operator<(const ConsoleLayer &rhs) const { return id < rhs.id; }
    };

    struct LastUsedFontSettings
    {
        std::string fontName;
        int charWidth;
        int charHeight;
    };

    Console *_mainConsole;
    std::set<ConsoleLayer> _layers; // applied on top of main

    static LastUsedFontSettings _lastUsedFontSettings;
    static unsigned int _layerIds;

public:
    TileEngine(const char *tileset, int tw, int th, int sw, int sh);
    ~TileEngine();

public:
    void randomizeTiles();
    void run();

    inline void setChar(int x, int y, unsigned char v) { _mainConsole->setChar(x, y, v); }
    inline void setCharForeground(int x, int y, Color fg) { _mainConsole->setCharForeground(x, y, fg); }
    inline void setCharBackground(int x, int y, Color bg) { _mainConsole->setCharBackground(x, y, bg); }

    Console* layer(const std::string &name) const;
    Console* addLayer(const std::string &name, int x = 0, int y = 0, int w = 0, int h = 0);

    void draw();

    static Console* createConsole(int w, int h);

    Console* mainConsole() { return _mainConsole; }
    const Console* mainConsole() const { return _mainConsole; }


};

#endif // __INCLUDE_TILE_ENGINE_H__