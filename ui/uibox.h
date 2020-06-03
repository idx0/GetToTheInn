#pragma once

#include "uiwidget.h"
#include <vector>

namespace ui {

class block : public widget
{
    int _icon;
    gtti::Color _color;
    Console *_console;

public:
    block(int icon, const gtti::Color &color, const Rect &rect, widget *parent = nullptr);
    virtual ~block();

    void draw(canvas* console);
};

class vbox : public widget
{
    std::vector<widget*> _children;
    int _top = 0;

public:
    vbox(const Rect &rect, widget *parent = nullptr);
    virtual ~vbox();

    virtual void addChild(widget* child);

    void draw(canvas* console);
    void drawChildren(canvas* console);
};

} // namespace ui