#ifndef PA1_UTIL_H
#define PA1_UTIL_H

#include <stdint.h>

#include "ipc.h"
#include "pipes_io.h"

uint8_t parse_arg (const char* arg);

Message *create_message(uint16_t magic, int16_t type);

Message *create_timed_message(uint16_t magic, int16_t type, uint16_t payload_len, timestamp_t local_time);

int receive_from_all_processes(io_channel_t* io_channel);

int send_started(io_channel_t* io_channel, Message* started_message);

int send_done(io_channel_t* io_channel, Message* done_message);

int wait_all_process_finished(io_channel_t* io_channel);

#endif //PA1_UTIL_H

