#ifndef _ROLLING_CODE_H_
#define ROLLING_CODE_H_
#include <cstdint>

uint32_t generateNextInt(uint32_t*);
void intToResponse(int, char []);
int responseToInt(char []);

#endif