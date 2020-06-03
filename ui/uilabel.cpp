#include <ctype.h>
#include <sstream>
#include <set>

#include "uilabel.h"

namespace ui {

	label::label(const std::string& sz, widget *parent) :
		widget(parent),
		m_text(sz),
		m_color(gtti::Color::white)
	{
	}

	label::~label()
	{
	}

	std::string label::wrap(const std::string& text, int margin)
	{
		std::istringstream words(text);
		std::ostringstream wrapped;
		std::string word;

		if (words >> word) {
			wrapped << word;
			int space = margin - word.length();

			while (words >> word) {
				if (space < (int)word.length() + 1) {
					wrapped << std::endl << word;
					space = margin - word.length();
				} else {
					wrapped << ' ' << word;
					space -= word.length() + 1;
				}
			}
		}

		return wrapped.str();
	}

	std::string wrap_it(const std::string& text, int margin)
	{
		std::string out;
		std::string word;
		int ws, we;
		bool inword = false;
		int ll = 0;

		for (int i = 0; i < (int)text.length(); i++) {
			char c = text.at(i);

			if ((isspace(c)) || (c == '\a')) {
				if (inword) {
					we = i;
					word = text.substr(ws, we - ws);

					int lensofar = ll + word.length();
					if (c == '\a') { lensofar++; }

					if (lensofar > margin) {
						out += '\n';
						out.append(word);
						word.clear();
					}
					inword = false;
				}
			} else {
				if (!inword) {
					ws = i;
					inword = true;
				}
			}
		}

		return out;
	}

	std::string wrap2(const std::string& text, int margin)
	{
		struct breakpoint {
			int cost;	// cost of this node
			int pos;	// positing in text
			struct breakpoint* parent;
			std::set<struct breakpoint*> children;
		};

		std::set<struct breakpoint*> active;

		breakpoint head;
		head.cost = 0;
		head.pos = 0;
		head.parent = NULL;

//		breakpoint *cur = &head, *child;

		return std::string();
	}

	void label::setLabel(const std::string& sz)
	{
		m_text = wrap(sz, m_size.width());
	}

	void label::setSize(const Rect &size)
	{
		widget::setSize(size);
		m_textRect = size.shrink(1);
		m_text = wrap(m_text, m_textRect.width());
	}

	void label::draw(canvas *console)
	{
		if (isVisible()) {
			console->drawText(m_textRect, m_text, m_color);

			drawChildren(console);
		}
	}

}
