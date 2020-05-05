#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "child_functions.h"
#include "pipes_io.h"
#include "ipc.h"
#include "log.h"
#include "pa2345.h"
#include "banking.h"
#include "lamport.h"
#include "util.h"

// sending messages

int send_started(io_channel_t *io_channel)
{
    inc_lamport_time();
    Message *started_message = create_timed_message(MESSAGE_MAGIC, STARTED, 0, get_lamport_time());
    sprintf(started_message->s_payload, log_started_fmt, get_lamport_time(), io_channel->id, getpid(), getppid(),
            io_channel->balance_history.s_history->s_balance);
    started_message->s_header.s_payload_len = (uint16_t)strlen(started_message->s_payload);
    if (!send_multicast(io_channel, started_message))
    {
        return 1;
    }
    return 0;
}

int send_done(io_channel_t *io_channel)
{
    inc_lamport_time();
    Message *done_message = create_timed_message(MESSAGE_MAGIC, DONE, 0, get_lamport_time());
    sprintf(done_message->s_payload, log_done_fmt, get_lamport_time(), io_channel->id,
            io_channel->balance_history.s_history->s_balance);
    done_message->s_header.s_payload_len = (uint16_t)strlen(done_message->s_payload);
    if (!send_multicast(io_channel, done_message))
    {
        return 1;
    }
    return 0;
}

int send_cs_request(io_channel_t *io_channel)
{
    inc_lamport_time();
    Message *cs_request_message = create_timed_message(MESSAGE_MAGIC, CS_REQUEST, 0, get_lamport_time());
    sprintf(cs_request_message->s_payload, log_done_fmt, get_lamport_time(), io_channel->id,
            io_channel->balance_history.s_history->s_balance);
    cs_request_message->s_header.s_payload_len = (uint16_t)strlen(cs_request_message->s_payload);
    if (!send_multicast(io_channel, cs_request_message))
    {
        return 1;
    }
    return 0;
}

int send_cs_reply_to(io_channel_t *io_channel, local_id reply_to)
{
    inc_lamport_time();
    Message *cs_reply_message = create_timed_message(MESSAGE_MAGIC, CS_REPLY, 0, get_lamport_time());
    sprintf(cs_reply_message->s_payload, log_done_fmt, get_lamport_time(), io_channel->id,
            io_channel->balance_history.s_history->s_balance);
    cs_reply_message->s_header.s_payload_len = (uint16_t)strlen(cs_reply_message->s_payload);
    if (!send(io_channel, reply_to, cs_reply_message))
    {
        return 1;
    }
    return 0;
}

// pa4 functions for algorithm 

int request_cs(const void * self)
{
    io_channel_t *io_channel = (io_channel_t *)self;
    
    send_cs_request(io_channel);
    add_to_lamport_queue(io_channel, get_lamport_time(), io_channel->id);

    // waiting for responses from other processes

    Message *msg = (Message*) malloc(sizeof(Message));
    uint8_t processes_we_wait_for = io_channel->children_num - 1; //children except for current

    while (processes_we_wait_for)
    {
        local_id msg_from = receive_any(io_channel, msg);
        set_max_lamport_time(msg->s_header.s_local_time);
        switch (msg->s_header.s_type)
        {
        case CS_REPLY:
           // another process received our cs request and added it to its local queue
            processes_we_wait_for -= 1;
        break;
        case CS_REQUEST:
            // another process wants cs, so add it to our local queue
            add_to_lamport_queue(io_channel, get_lamport_time(), msg_from);
            // send him reply that we successfully received the message and added to queue
            send_cs_reply_to(io_channel, msg_from);
        break;
        default:
        break;
        }
    }

}
