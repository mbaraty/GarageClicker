//
// Created by mbara on 7/25/2025.
//

#ifndef FRAMUTILS_H
#define FRAMUTILS_H
#include <definitions.h>

#ifdef TRIGGER_MODE
#include "FRAM.h"

void writeGstate(FRAM*, uint32_t);

uint32_t readGstate(FRAM*);

#endif
#endif //FRAMUTILS_H
