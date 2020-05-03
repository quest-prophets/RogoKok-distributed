#include "log.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdarg.h>

#include "common.h"
#include "pa2345.h"
#include "pipes_io.h"
#include "lamport.h"

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
    vfprintf(events_log_file, fmt, va_fargs);

    va_end(va_args);
    va_end(va_fargs);
}

void log_pipe_open(local_id from, local_id to)
{
    assert(pipes_log_file != NULL);

    //printf("Process pipe %d -> %d OPENED\n", from, to);
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
    log_printf(log_started_fmt, 
                get_lamport_time(), 
                io_channel->id, 
                pid, 
                parent_pid, 
                io_channel->balance_history.s_history[io_channel->balance_history.s_history_len - 1].s_balance);
}

void log_received_all_started(io_channel_t* io_channel)
{
    log_printf(log_received_all_started_fmt, get_lamport_time(), io_channel->id);
}

void log_done(io_channel_t* io_channel)
{
    log_printf(log_done_fmt, get_lamport_time(), io_channel->id, 
                io_channel->balance_history.s_history[io_channel->balance_history.s_history_len - 1].s_balance);
}

void log_received_all_done(io_channel_t* io_channel)
{
    log_printf(log_received_all_done_fmt, get_lamport_time(), io_channel->id);
}

void log_transfer_in(TransferOrder* transfer_order)
{
    log_printf(log_transfer_in_fmt, get_lamport_time(), transfer_order->s_dst,
            transfer_order->s_amount, transfer_order->s_src);
}

void log_transfer_out(TransferOrder* transfer_order)
{
    log_printf(log_transfer_out_fmt, get_lamport_time(), transfer_order->s_src,
               transfer_order->s_amount, transfer_order->s_dst);
}

