//
// Created by mbara on 7/25/2025.
//

#ifdef TRIGGER_MODE
#include "FramUtils.h"
void writeGstate(FRAM* fram, uint32_t value)
{
    fram -> write32(FRAM_GSTATE_ADDR, value);
}

uint32_t readGstate(FRAM* fram)
{
    return fram -> read32(FRAM_GSTATE_ADDR);
}


#endif
