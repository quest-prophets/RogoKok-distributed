#ifndef PA1_UTIL_H
#define PA1_UTIL_H

#include <stdint.h>

#include "ipc.h"

uint8_t parse_children_num (const char* arg);

Message *create_message(uint16_t magic, int16_t type);

#endif //PA1_UTIL_H
