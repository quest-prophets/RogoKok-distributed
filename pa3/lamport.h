#ifndef PA3_LAMPORT_H
#define PA3_LAMPORT_H

#include "ipc.h"
#include "banking.h"

void inc_lamport_time();

void set_max_lamport_time(timestamp_t another_time);

#endif //PA3_LAMPORT_H
