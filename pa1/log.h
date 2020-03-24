#ifndef __IFMO_DISTRIBUTED_CLASS_PA1_LOG__H
#define __IFMO_DISTRIBUTED_CLASS_PA1_LOG__H

#include "ipc.h"

void log_init();

void log_started();

void log_received_all_started();

void log_done();

void log_received_all_done();

void log_printf(const char *fmt, ...);

void log_pipe_open(local_id from, local_id to);

void log_events_close();

void log_pipes_close();

#endif // __IFMO_DISTRIBUTED_CLASS_PA1_LOG__H