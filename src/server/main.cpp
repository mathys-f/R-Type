#include "game_engine/engine.h"

int main()
{
    engn::EngineContext engine_ctx;

    while (true) {
        // Update engine context
        engine_ctx.run_systems();
    }

    return 0;
}