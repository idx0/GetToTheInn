#include <time.h>

#include "engine.h"
#include "rnd.h"

#include "sys/logger.h"
#include "sys/eventqueue.h"
#include "sys/eof_parser.h"

#include "libtcod/libtcod.hpp"

#define TEST

int main(int argc, char **argv)
{
	sys::logger::createLogger("gtti.log");
	sys::eventqueue::createEventQueue();

	Rnd::seed(time(NULL));

#ifndef TEST
	Engine::init();
#else
	sys::eof::PETest();
#endif

#ifndef TEST
	while ((!Engine::quit()) && 
		   (!TCODConsole::isWindowClosed())) {
		Engine::checkForInput();

		// render layers
		//   -- 4: cursor      --
		//   -- 3: gui         --
		//   -- 2: map fx      --
		//	 -- 2b: weather?   --
		//   -- 1: map objects --
		//	 -- 0: ground fx   --

		//TCODConsole::flush();
	}

	Engine::final();
#endif

	return 0;
}
