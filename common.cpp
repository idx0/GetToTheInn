#include "common.h"

SpecMap& SpecMap::operator=(const SpecMap& rhs)
{
	if (this != &rhs) {
		position		= rhs.position;
		walkable		= rhs.walkable;
		transparent		= rhs.transparent;
		seen			= rhs.seen;
		heard			= rhs.heard;
		fullIfVisible	= rhs.fullIfVisible;
		explored		= rhs.explored;
		lightLevel		= rhs.lightLevel;
		ambientLevel	= rhs.ambientLevel;
	}

	return *this;
}
