#ifndef PA1_UTIL_H
#define PA1_UTIL_H

#include <stdint.h>

#include "ipc.h"
#include "pipes_io.h"

uint8_t parse_arg (const char* arg);

Message *create_message(uint16_t magic, int16_t type);

Message *create_timed_message(uint16_t magic, int16_t type, uint16_t payload_len, timestamp_t local_time);

int receive_from_all_processes(io_channel_t* io_channel, int16_t type);

void add_to_lamport_queue(io_channel_t *io_channel, timestamp_t time, local_id pid);

int get_lowest_time_request_num(io_channel_t *io_channel);

void pop_from_lamport_queue(io_channel_t *io_channel);

#endif //PA1_UTIL_H

