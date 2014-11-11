#include "object.h"
#include "rnd.h"

Object::Object(const Point& pos, int icon, const Color& color) :
	m_position(pos),
	m_icon(icon),
	m_fgColor(color),
	m_light(NULL)
{
}


Object::Object(int x, int y, int icon, const Color& color) :
	m_position(x, y),
	m_icon(icon),
	m_fgColor(color),
	m_light(NULL)
{
}


Object::~Object()
{
}


Point Object::coords() const
{
	return m_position;
}

Tile Object::tile() const
{
	Tile t(m_fgColor, m_icon);
	t.bgColor = m_bgColor;

	return t;
}

LightingModel Object::lightingModel() const
{
	return m_lightingModel;
}

MobilityModel Object::mobilityModel() const
{
	return m_mobilityModel;
}

///////////////////////////////////////////////////////////////////////////////

MagicTree::MagicTree(const Point& p) : Object(p, 5, Color::lerp(Color::sea, Color::neon, Rnd::rndn()))
{
	m_bgColor = Color::lerp(Color(12, 8, 4), Color(0, 8, 4), Rnd::rndn());

	m_light = new Light(m_position.x(), m_position.y(), Rnd::betweenf(0.8f, 1.2f), Rnd::between(12,16));
	m_light->color = m_fgColor;

	m_mobilityModel.flags |= M_REACHABLE;

	m_lightingModel.ambientColor = Color(96, 96, 96);
	m_lightingModel.flags |= L_ALWAYS_LIT;

	m_flavor = std::string("a large tree glowing with magical energy");
}

MagicTree::~MagicTree()
{
	delete m_light;
}

///////////////////////////////////////////////////////////////////////////////

MagicShroom::MagicShroom(const Point& p) : Object(p, 140, Color::lerp(Color::violet, Color::han, Rnd::rndn()))
{
	m_bgColor = Color::lerp(Color(12, 8, 4), Color(0, 8, 4), Rnd::rndn());

	m_light = new Light(m_position.x(), m_position.y(),
						Rnd::betweenf(0.8f, 1.25f), Rnd::between(2,3),
						Color::lerp(Color::han, Color::purple, Rnd::rndn()));

	m_mobilityModel.flags |= (M_WALKABLE | M_JUMPABLE | M_REACHABLE);

	m_lightingModel.ambientColor = Color(96, 96, 96);
	m_lightingModel.flags |= L_TRANSPARENT;

	m_flavor = std::string("a strange looking fungus, possibly with magical properties");
}

MagicShroom::~MagicShroom()
{
	delete m_light;
}
