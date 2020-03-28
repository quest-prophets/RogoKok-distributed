#include "log.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <zconf.h>

#include "common.h"
#include "pa1.h"
#include "pipes_io.h"

FILE *events_log_file;
FILE *pipes_log_file;

void log_init()
{
    events_log_file = fopen(events_log, "a+");
    pipes_log_file = fopen(pipes_log, "w+");
}

void log_printf(const char *fmt, ...)
{
    assert(events_log_file != NULL);

    va_list va_args, va_fargs;

    va_start(va_args, fmt);
    va_copy(va_fargs, va_args);

    vprintf(fmt, va_args);
    vfprintf(events_log_file, fmt, va_args);

    va_end(va_args);
    va_end(va_fargs);
}

void log_pipe_open(local_id from, local_id to)
{
    assert(pipes_log_file != NULL);

    printf("Process pipe %d -> %d OPENED\n", from, to);
    fprintf(pipes_log_file, "Process pipe %d -> %d OPENED\n", from, to);
}

void log_events_close()
{
    assert(events_log_file != NULL);

    fclose(events_log_file);
}

void log_pipes_close()
{
    assert(pipes_log_file != NULL);

    fclose(pipes_log_file);
}

void log_started(io_channel_t* io_channel)
{
    pid_t pid = getpid();
    pid_t parent_pid = getppid();
    log_printf(log_started_fmt, io_channel->id, pid, parent_pid);
}

void log_received_all_started(io_channel_t* io_channel)
{
    log_printf(log_received_all_started_fmt, io_channel->id);
}

void log_done(io_channel_t* io_channel)
{
    log_printf(log_done_fmt, io_channel->id);
}

void log_received_all_done(io_channel_t* io_channel)
{
    log_printf(log_received_all_done_fmt, io_channel->id);
}