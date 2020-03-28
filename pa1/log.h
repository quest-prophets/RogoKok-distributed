#ifndef __IFMO_DISTRIBUTED_CLASS_PA1_LOG__H
#define __IFMO_DISTRIBUTED_CLASS_PA1_LOG__H

#include "ipc.h"
#include "pipes_io.h"

void log_init();

void log_started(io_channel_t* io_channel);

void log_received_all_started(io_channel_t* io_channel);

void log_done(io_channel_t* io_channel);

void log_received_all_done(io_channel_t* io_channel);

void log_printf(const char *fmt, ...);

void log_pipe_open(local_id from, local_id to);

void log_events_close();

void log_pipes_close();

#endif // __IFMO_DISTRIBUTED_CLASS_PA1_LOG__H

