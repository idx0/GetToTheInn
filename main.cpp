#include <time.h>

#include "engine.h"
#include "rnd.h"

#include "sys/logger.h"
#include "sys/eventqueue.h"
#include "sys/eof_parser.h"

#include "TileEngine.h"

int main(int argc, char **argv)
{
    sys::logger::createLogger("gtti.log");
    sys::eventqueue::createEventQueue();

    Rnd::seed(time(NULL));

#ifndef TEST
    printf("EE: go!\n");

    Engine::init();
#else
    sys::eof::PETest();
#endif
    Engine::update(0);

    printf("EE: run!\n");

    while (!Engine::quit() && !WindowShouldClose()) {
        Engine::checkForInput();
        Engine::render();
    }

	Engine::final();

	return 0;
}
