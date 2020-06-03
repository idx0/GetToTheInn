#include "uibox.h"
#include "../TileEngine.h"

#include <algorithm>

namespace ui {

block::block(int icon, const gtti::Color &color, const Rect &rect, widget *parent) : widget(parent), _icon(icon), _color(color)
{
    _console = TileEngine::createConsole(rect.width(), rect.height());
    setSize(rect);

    for (int x = 0; x < _console->width(); x++) {
        for (int y = 0; y < _console->height(); y++) {
            _console->at(x, y).setChar(icon);
            _console->at(x, y).setForegroundColor(color.toColor());
        }
    }
}

block::~block()
{

}

void block::draw(canvas* console)
{
    if (isVisible()) {
        console->console()->apply(_console, m_size.left(), m_size.top());
    }
}

vbox::vbox(const Rect &rect, widget *parent) : widget(parent)
{
    setSize(rect);
}

vbox::~vbox()
{
    for (auto child : _children) {
        delete child;
    }
}

void vbox::addChild(widget* child)
{
    int b = child->getSize().height();
    int r = child->getSize().width();

    if (_top < m_size.height()) {
        b = std::min(m_size.height(), _top + child->getSize().height());
        r = std::min(m_size.width(), child->getSize().width());
    }

    Rect pos(_top, 0, b, r);
    pos.move(m_size.left(), m_size.top());

    child->setSize(pos);
    _children.push_back(child);

    _top = b;
}

void vbox::draw(canvas* console)
{
    if (isVisible()) {
        // draw children to the hbox console
        drawChildren(console);
    }
}

void vbox::drawChildren(canvas* console)
{
    for (auto child : _children) {
        if (child) {
            child->draw(console);
        }
    }
}

} // namespace ui
