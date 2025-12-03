#include "engine.h"

using namespace engn;

void EngineContext::run_systems()
{
    for (auto& sys : m_systems) {
        if (sys)
            sys(*this);
    }
}
