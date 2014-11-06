#pragma once

#include "uiwidget.h"

namespace ui {

	class label : public widget
	{
	public:
		explicit label(const std::string& sz, widget *parent = NULL);
		virtual ~label();

		virtual void draw(canvas *console);

		void setLabel(const std::string& sz) { m_text = sz; }
		void setTextColor(const Color& c) { m_color = c; }

		std::string getText() const;
	protected:

		std::string m_text;
		Color m_color;
	};

	inline
	std::string label::getText() const
	{
		return m_text;
	}
}
