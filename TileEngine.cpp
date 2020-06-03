#include "TileEngine.h"

#include "common.h"

#include <algorithm>
#include <stdio.h>

#define _USE_MATH_DEFINES
#include <math.h>

TileEngine::LastUsedFontSettings TileEngine::_lastUsedFontSettings;
unsigned int TileEngine::_layerIds = 0;


PsychedlicFilter::PsychedlicFilter() : changeTick(FPS)
{
}

PsychedlicFilter::~PsychedlicFilter()
{
}

Icon PsychedlicFilter::operator()(const Icon &i, int x, int y)
{
    static const gtti::Gradient g(gtti::Color(232, 12, 223), gtti::Color(191, 0, 255), gtti::Color(117, 12, 232));
    static const float alphaRange = 0.25f;
    static const float alphaMin = 0.05f;

    Icon ret = i;

    float alpha = alphaMin + alphaRange * fabs(sin(alphaFreq * tick + alphaPhase));
    float blend = fabs(sin(blendFreq * tick + blendPhase));

    if (x == 0 && y == 0) {
        tick += (1.0f / (float)FPS);

        if (changeTick.tick()) {
            changeTick.restart(Rnd::betweenf(1.0, 3.0) * FPS);
            alphaFreq = Rnd::betweenf(0.5, 3.5);
        }
    }

    ret._bg = gtti::Color::blend(gtti::Color(ret._bg), g.getColor(blend), alpha).toColor();
    ret._fg = gtti::Color::blend(gtti::Color(ret._fg), g.getColor(blend), alpha).toColor();

    return ret;
}

FadeFilter::FadeFilter(float out, float hold, float in, const gtti::Color &c) :
    _in(in * FPS), _out(out * FPS), _hold(hold * FPS), _c(c)
{

}

FadeFilter::~FadeFilter()
{

}

Icon FadeFilter::operator()(const Icon &i, int x, int y)
{
    if (x == 0 && y == 0) {
        switch (_state) {
            case FF_OUT:
            {
                _alpha = _out.percent();
                if (_out.tick()) {
                    _state = FF_HOLD;
                }
                break;
            }
            case FF_IN:
            {
                _alpha = 1.0f - _in.percent();
                if (_in.tick()) {
                    _state = FF_IDLE;
                }
                break;
            }
            case FF_HOLD:
            {
                _alpha = 1.0f;
                if (_hold.tick()) {
                    _state = FF_IN;
                }
                break;
            }
            default:
                _alpha = 0.0f;
                break;
        }
    }

    Icon ret = i;

    ret._bg = gtti::Color::blend(gtti::Color(ret._bg), _c, _alpha).toColor();
    ret._fg = gtti::Color::blend(gtti::Color(ret._fg), _c, _alpha).toColor();

    return ret;
}

ThunderstormFilter::ThunderstormFilter() :
    _lightening(0.1f, 0.0f, 0.5f, gtti::Color::white),
    _rain(FPS * Rnd::betweenf(3.5f, 6.0f)),
    _rate(6)
{

}

ThunderstormFilter::~ThunderstormFilter()
{

}

Icon ThunderstormFilter::operator()(const Icon &i, int x, int y)
{
    static const unsigned int offset = 43;

    _count++;

    if (x == 0 && y == 0) {
        if (_isRaining) {
            if (_rain.tick()) {
                _isRaining = !Rnd::one_in(3);
                _rain.restart(FPS * Rnd::betweenf(2.0f, 5.0f));
                _rate.reset();
            } else {
                if (_rate.tick()) {
//                    _offset++;
                }
            }
        } else {
            if (_lightening.done()) {
                _isRaining = true;
                _lightening.reset();
            }
        }
    }

    if (y == 0) {
        _count = x + _tick++;
    }

    if (_isRaining) {
        Icon ret = i;

        if ((_count % offset) == 0 && Rnd::one_in(3)) {
            ret._val = '\'';// 176;
            ret._fg = gtti::Color(131, 165, 255).toColor();
        }

        ret._fg = gtti::Color::blend(gtti::Color(ret._fg), gtti::Color(77, 80, 87), 0.2f).toColor();
        ret._bg = gtti::Color::blend(gtti::Color(ret._bg), gtti::Color(77, 80, 87), 0.2f).toColor();

        return ret;
    } else {
        return _lightening(i, x, y);
    }
}



void Console::Tile::setDimensions(int w, int h)
{
    _size = Size(w, h);
}

void Console::Tile::draw(Filter *filter, Texture2D tex, int x, int y) const
{
    Vector2 pos;
    Rectangle frm;

    Icon icon = _icon;
    if (filter) {
        icon = (*filter)(_icon, x, y);
    }

    frm.x = _size.width() * (icon._val % 16);
    frm.y = _size.height() * (icon._val / 16);
    frm.width = _size.width();
    frm.height = _size.height();

    pos.x = x;
    pos.y = y;

    if (icon._bg.a != 0) {
        DrawRectangle(pos.x, pos.y, _size.width(), _size.height(), icon._bg);
    }

    if (icon._val == 0 || icon._val == ' ') return;

    DrawTextureRec(tex, frm, pos, icon._fg);
}


Console::Console(const char *font, int cw, int ch, int sw, int sh) : _pos(0, 0), _size(sw, sh), _csize(cw, ch)
{
    _table = new Console::Tile[sw * sh];

    Image img = LoadImage(font);
    ImageAlphaMask(&img, img);

    //Texture2D ascii = LoadTexture("ascii.png");
    _texture.load(img);

    UnloadImage(img);

    for (int i = 0; i < (sw * sh); i++) {
        _table[i].setDimensions(cw, ch);
        _table[i].setChar(' ');
    }
}

Console::Console(int w, int h) : _pos(0, 0), _size(w, h)
{
    _table = new Console::Tile[w * h];
}

Console::Console(const Console &c, int w, int h) : _texture(c._texture), _font(c._font), _pos(c._pos), _size(w, h), _csize(c._csize)
{
    _table = new Console::Tile[w * h];

    for (int i = 0; i < (w * h); i++) {
        _table[i].setDimensions(_csize.width(), _csize.height());
        _table[i].setChar(' ');
    }
}

Console::~Console()
{
    delete[] _table;
}

void Console::getCharSize(int *w, int *h)
{
    if (w) {
        *w = _csize.width();
    }

    if (h) {
        *h = _csize.height();
    }
}

void Console::draw()
{
    if (!_texture) return;
    if (!_visible) return;

    for (int x = 0; x < _size.width(); x++) {
        for (int y = 0; y < _size.height(); y++) {
            int px = (x + _pos.x()) * _csize.width();
            int py = (y + _pos.y()) * _csize.height();
            at(x, y).draw(_filter, _texture, px, py);
        }
    }
}

void Console::clear()
{
    for (int x = 0; x < _size.width(); x++) {
        for (int y = 0; y < _size.height(); y++) {
            at(x, y) = Tile();
            at(x, y).setDimensions(_csize.width(), _csize.height());
            at(x, y).setBackgroundColor(_bg.toColor());
        }
    }
}

void Console::apply(Console *other, int x, int y)
{
    for (int ix = x; (ix < _size.width() && ((ix - x) < other->width())); ix++) {
        for (int iy = y; (iy < _size.height() && ((iy - y) < other->height())); iy++) {
            Tile o = other->at(ix - x, iy - y);

            at(ix, iy).setChar(o.getChar());
            at(ix, iy).setForegroundColor(o.getForegroundColor());
            at(ix, iy).setBackgroundColor(o.getBackgroundColor());
        }
    }
}

#define BLENDNORM(x) (((float)x) / 255.0f)

void Console::blend(Console *other, int x, int y, float fgBlend, float bgBlend)
{
    for (int ix = x; (ix < _size.width() && ((ix - x) < other->width())); ix++) {
        for (int iy = y; (iy < _size.height() && ((iy - y) < other->height())); iy++) {
            Tile t = at(ix, iy);
            Tile o = other->at(ix - x, iy - y);
            Color fgd = t.getForegroundColor();
            Color fgs = o.getForegroundColor();
            Color bgd = t.getBackgroundColor();
            Color bgs = o.getBackgroundColor();

            Color fg, bg;

            fg.a = 255 * (BLENDNORM(fgs.a) + BLENDNORM(fgd.a) * (1 - BLENDNORM(fgs.a)));
            fg.r = (fgs.r * BLENDNORM(fgs.a) + fgd.r * (BLENDNORM(fgd.a) * (1 - BLENDNORM(fgs.a)))) / BLENDNORM(fg.a);
            fg.g = (fgs.g * BLENDNORM(fgs.a) + fgd.g * (BLENDNORM(fgd.a) * (1 - BLENDNORM(fgs.a)))) / BLENDNORM(fg.a);
            fg.b = (fgs.b * BLENDNORM(fgs.a) + fgd.b * (BLENDNORM(fgd.a) * (1 - BLENDNORM(fgs.a)))) / BLENDNORM(fg.a);

            bg.a = 255 * (BLENDNORM(bgs.a) + BLENDNORM(bgd.a) * (1 - BLENDNORM(bgs.a)));
            bg.r = (bgs.r * BLENDNORM(bgs.a) + bgd.r * (BLENDNORM(bgd.a) * (1 - BLENDNORM(bgs.a)))) / BLENDNORM(bg.a);
            bg.g = (bgs.g * BLENDNORM(bgs.a) + bgd.g * (BLENDNORM(bgd.a) * (1 - BLENDNORM(bgs.a)))) / BLENDNORM(bg.a);
            bg.b = (bgs.b * BLENDNORM(bgs.a) + bgd.b * (BLENDNORM(bgd.a) * (1 - BLENDNORM(bgs.a)))) / BLENDNORM(bg.a);

            at(ix, iy).setChar(o.getChar());
            at(ix, iy).setForegroundColor(fg);
            at(ix, iy).setBackgroundColor(bg);
        }
    }
}

#undef BLENDNORM

void Console::setFilter(Filter *f)
{
    if (_filter) {
        delete _filter;
    }

    _filter = f;
}

TileEngine::TileEngine(const char *tileset, int tw, int th, int sw, int sh)
{
    InitWindow(sw * tw, sh * th, "gtti");

    _mainConsole = new Console(tileset, tw, th, sw, sh);
    _lastUsedFontSettings.fontName = std::string(tileset);
    _lastUsedFontSettings.charWidth = tw;
    _lastUsedFontSettings.charHeight = th;

    SetTargetFPS(FPS);
}

TileEngine::~TileEngine()
{
    delete _mainConsole;
    CloseWindow();

    for (auto l : _layers) {
        delete l.console;
    }
}

Console* TileEngine::layer(const std::string &name) const
{
    for (auto l : _layers) {
        if (l.name == name) {
            return l.console;
        }
    }

    return nullptr;
}

Console* TileEngine::addLayer(const std::string &name, int x, int y, int w, int h)
{
    ConsoleLayer l;
    l.id = _layerIds++;
    l.pos = Rect(x, y, Size(w, h));
    l.console = (_mainConsole ? new Console(*_mainConsole, w, h) : nullptr);
    l.name = name;

    if (l.console) {
        l.console->setPosition(Point(x, y));
    }

    _layers.insert(l);

    return l.console;
}

void TileEngine::randomizeTiles()
{
    // random values
    for (int x = 0; x < _mainConsole->width(); x++) {
        for (int y = 0; y < _mainConsole->height(); y++) {
            Color fg, bg;

            fg.a = 255;
            fg.r = GetRandomValue(100, 255);
            fg.g = GetRandomValue(100, 255);
            fg.b = GetRandomValue(100, 255);

            bg.a = 255;
            bg.r = GetRandomValue(0, 205);
            bg.g = GetRandomValue(0, 205);
            bg.b = GetRandomValue(0, 205);

            _mainConsole->at(x, y).setChar(GetRandomValue(0, 255));
            _mainConsole->at(x, y).setForegroundColor(fg);
            _mainConsole->at(x, y).setBackgroundColor(bg);
        }
    }
}

void TileEngine::draw()
{
    if (_mainConsole) {
        _mainConsole->draw();
    }

    for (auto l : _layers) {
        if (l.console) {
            l.console->draw();
        }
    }
}

void TileEngine::run()
{
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        draw();
        EndDrawing();
    }
}

Console* TileEngine::createConsole(int w, int h)
{
    //return new Console(_lastUsedFontSettings.fontName.c_str(), _lastUsedFontSettings.charWidth, _lastUsedFontSettings.charHeight, w, h);
    return new Console(w, h);
}
