#pragma once

#include <vector>
#include <string>

#include "common.h"
#include "lighting.h"

class Object
{
public:
	Object(const Point& pos, int icon, const Color& color);
	Object(int x, int y, int icon, const Color& color);
	virtual ~Object();

	virtual void update() {}
	virtual void interact() {}
	virtual std::string flavor() { return m_flavor; }

public:

	virtual Point coords() const;
	virtual Tile tile() const;

	virtual LightingModel lightingModel() const;
	virtual MobilityModel mobilityModel() const;

protected:
	// elementary properties
	Point m_position;

	// Tile
	int m_icon;
	Color m_fgColor;
	Color m_bgColor;

	// lighting properties
	LightingModel m_lightingModel;

	// mobility properties
	MobilityModel m_mobilityModel;
	
	// object light (can be NULL if the object does not give off light)
	Light* m_light;

	std::string m_flavor;

private:
	// object id (TODO??)
	unsigned int id;
};

typedef std::vector<Object*> ObjectMap;


///////////////////////////////////////////////////////////////////////////////

class MagicTree : public Object
{
public:
	MagicTree(const Point& p);
	~MagicTree();
};

class MagicShroom : public Object
{
public:
	MagicShroom(const Point& p);
	~MagicShroom();
};
