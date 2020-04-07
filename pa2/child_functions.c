#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "child_functions.h"
#include "pipes_io.h"
#include "ipc.h"
#include "pa2345.h"
#include "banking.h"

int send_started(io_channel_t* io_channel, Message* started_message)
{
    sprintf(started_message->s_payload, log_started_fmt, get_physical_time(), io_channel->id, getpid(), getppid(),
            io_channel->balance_history.s_history->s_balance);
    started_message->s_header.s_payload_len = (uint16_t) strlen(started_message->s_payload);
    if (!send_multicast(io_channel, started_message))
    {
        return 1;
    }
    return 0;
}

int send_done(io_channel_t* io_channel, Message* done_message)
{
    sprintf(done_message->s_payload, log_done_fmt, get_physical_time(), io_channel->id,
            io_channel->balance_history.s_history->s_balance);
    done_message->s_header.s_payload_len = (uint16_t) strlen(done_message->s_payload);
    if (!send_multicast(io_channel, done_message))
    {
        return 1;
    }
    return 0;
}