//
// Created by mbara on 7/25/2025.
//

#ifndef DEBUG_H
#define DEBUG_H

#define DEBUG 1

#ifdef DEBUG
#define DEBUG_PRINTF(...) Serial.printf(__VA_ARGS__)
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
#else
#define DEBUG_PRINTF(...)
#define DEBUG_PRINTLN(...)
#define DEBUG_PRINT(...)
#endif
#endif //DEBUG_H
