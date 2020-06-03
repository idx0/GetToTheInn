#pragma once

#include <vector>
#include <string>

#include "common.h"
#include "lighting.h"

class Object
{
public:
	Object(const Point& pos, int icon, const gtti::Color & color);
	Object(int x, int y, int icon, const gtti::Color & color);
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
    gtti::Color  m_fgColor;
    gtti::Color  m_bgColor;

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

class NamedObject : public Object
{
public:
    NamedObject(const std::string &name, const Point &pos, int icon, const gtti::Color &color) : Object(pos, icon, color), m_name(name) {}
    NamedObject(const std::string &name, int x, int y, int icon, const gtti::Color &color) : Object(x, y, icon, color), m_name(name) {}
    ~NamedObject() {}

    const std::string& name() const { return m_name; }

protected:
    std::string m_name;
};

///////////////////////////////////////////////////////////////////////////////

class MagicTree : public Object
{
public:
	MagicTree(const Point& p);
	~MagicTree();
};

class MagicShroom : public NamedObject
{
public:
	MagicShroom(const Point& p);
	~MagicShroom();
};
