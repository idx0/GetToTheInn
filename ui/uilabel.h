#pragma once

#include "uiwidget.h"

namespace ui {

	class label : public widget
	{
	public:
		explicit label(const std::string& sz, widget *parent = NULL);
		virtual ~label();

		virtual void draw(canvas *console);

		void setLabel(const std::string& sz);
		void setTextColor(const gtti::Color& c) { m_color = c; }
		void setSize(const Rect &size);

		std::string getText() const;
	protected:

		std::string wrap(const std::string& text, int margin);

		std::string m_text;
		Rect m_textRect;
        gtti::Color m_color;
	};

	inline
	std::string label::getText() const
	{
		return m_text;
	}
}
