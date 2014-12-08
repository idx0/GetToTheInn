#pragma once

#include "libtcod/console_types.h"
#include <string>
#include <string.h>
#include <ctype.h>

static const std::string sTCOD_KEY_STRING[] = {
	"NONE",
	"ESC",		// escape
	"BS",		// backspace
	"TAB",
	"ENTER",
	"SHIFT",
	"CTRL",		// control
	"ALT",
	"PAUSE",
	"CAPS",		// capslock
	"PGUP",		// page up
	"PGDN",		// page down
	"END",
	"HOME",
	"UP",
	"LEFT",
	"RIGHT",
	"DOWN",
	"PRINTSCREEN",
	"INS",		// insert
	"DEL",		// delete
	"LWIN",
	"RWIN",
	"APPS",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"KP0",
	"KP1",
	"KP2",
	"KP3",
	"KP4",
	"KP5",
	"KP6",
	"KP7",
	"KP8",
	"KP9",
	"KPADD",
	"KPSUB",
	"KPDIV",
	"KPMUL",
	"KPDEC",
	"KPENTER",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12",
	"NUMLOCK",
	"SCROLLLOCK",
	"SPACE",
	"CHAR"
};

class Key
{
public:
	enum KeyModifier
	{
		ModifierNone      = 0,
		ModifierShift     = 1,
		ModifierLeftAlt   = 2,
		ModifierLeftCtrl  = 4,
		ModifierRightAlt  = 8,
		ModifierRightCtrl = 16
	};

public:
	Key(const TCOD_keycode_t keycode, const KeyModifier mods = ModifierNone)
	{
		memset(&m_key, 0, sizeof(TCOD_key_t));
		modifierToKey(mods);
		m_key.vk = keycode;
		m_key.c = 0;
	}
	Key(const TCOD_key_t& key)
	{
		memset(&m_key, 0, sizeof(TCOD_key_t));
		m_key.vk      = key.vk;
		m_key.c       = key.c;
		m_key.lalt    = key.lalt;
		m_key.lctrl   = key.lctrl;
		m_key.ralt    = key.ralt;
		m_key.rctrl   = key.rctrl;
		m_key.shift   = key.shift;
		m_key.pressed = key.pressed;
	}
	Key(const char c, const KeyModifier mods = ModifierNone)
	{
		memset(&m_key, 0, sizeof(TCOD_key_t));
		modifierToKey(mods);
		m_key.vk = TCODK_CHAR;
		m_key.c = c;
	}
	Key() { memset(&m_key, 0, sizeof(TCOD_key_t)); }
	~Key() { }

	bool isShift() const { return (m_key.shift != 0); }
	bool isLeftAlt() const { return (m_key.lalt != 0); }
	bool isLeftCtrl() const { return (m_key.lctrl != 0); }
	bool isRightAlt() const { return (m_key.ralt != 0); }
	bool isRightCtrl() const { return (m_key.rctrl != 0); }
	bool isPressed() const { return (m_key.pressed != 0); }
	bool isReleased() const { return !isPressed(); }

	bool isKey(const char c) const
	{
		return ((m_key.vk == TCODK_CHAR) && (tolower(m_key.c) == tolower(c)));
	}

	bool isKey(const TCOD_keycode_t code) const
	{
		return (m_key.vk == code);
	}

	bool isKey(const Key& key) const
	{
		return (operator==(key));
	}

	Key& operator=(const Key& rhs)
	{
		if (this != &rhs) {
			m_key.vk      = rhs.m_key.vk;
			m_key.c       = rhs.m_key.c;
			m_key.lalt    = rhs.m_key.lalt;
			m_key.lctrl   = rhs.m_key.lctrl;
			m_key.ralt    = rhs.m_key.ralt;
			m_key.rctrl   = rhs.m_key.rctrl;
			m_key.shift   = rhs.m_key.shift;
			m_key.pressed = rhs.m_key.pressed;
		}

		return *this;
	}

	bool operator==(const Key& rhs) const
	{
		// don't check .pressed!
		bool modEq = ((m_key.lalt    == rhs.m_key.lalt) &&
					  (m_key.lctrl   == rhs.m_key.lctrl) &&
					  (m_key.ralt    == rhs.m_key.ralt) &&
					  (m_key.rctrl   == rhs.m_key.rctrl) &&
					  (m_key.shift   == rhs.m_key.shift));
		bool eq = false;

		if (m_key.vk == TCODK_CHAR) {
			eq = (m_key.c == rhs.m_key.c) && modEq;
		} else {
			eq = (m_key.vk == rhs.m_key.vk) && modEq;
		}

		return eq;
	}
	bool operator!=(const Key& rhs) const
	{
		return (false == operator==(rhs));
	}

	std::string toString() const
	{
		std::string ret;

		if (m_key.vk == TCODK_CHAR) {
			ret.assign(1, m_key.c);
		} else  {
			ret.assign(sTCOD_KEY_STRING[m_key.vk]);
		}

		return ret;
	}
protected:
	void modifierToKey(const KeyModifier mods)
	{
		if (mods & ModifierShift) { m_key.shift = true; }
		if (mods & ModifierLeftAlt) { m_key.lalt = true; }
		if (mods & ModifierLeftCtrl) { m_key.lctrl = true; }
		if (mods & ModifierRightAlt) { m_key.ralt = true; }
		if (mods & ModifierRightCtrl) { m_key.rctrl = true; }
	}

	TCOD_key_t m_key;
};
