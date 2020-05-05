#ifndef PA3_LAMPORT_H
#define PA3_LAMPORT_H

#include "ipc.h"
#include "banking.h"

typedef struct 
{
    timestamp_t request_time;
    local_id pid;
} __attribute__((packed)) lamport_queue_record_t;


void inc_lamport_time();

void set_max_lamport_time(timestamp_t another_time);

void add_to_lamport_queue(io_channel_t *io_channel, timestamp_t time, local_id pid);

int get_lowest_time_request_num(io_channel_t *io_channel);

#endif //PA3_LAMPORT_H
