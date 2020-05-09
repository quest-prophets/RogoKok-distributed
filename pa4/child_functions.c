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

int send_cs_release(io_channel_t *io_channel)
{
    inc_lamport_time();
    Message *cs_release_message = create_timed_message(MESSAGE_MAGIC, CS_RELEASE, 0, get_lamport_time());
    sprintf(cs_release_message->s_payload, log_done_fmt, get_lamport_time(), io_channel->id,
            io_channel->balance_history.s_history->s_balance);
    cs_release_message->s_header.s_payload_len = (uint16_t)strlen(cs_release_message->s_payload);
    if (!send_multicast(io_channel, cs_release_message))
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

int request_cs(const void *self)
{
    io_channel_t *io_channel = (io_channel_t *)self;

    send_cs_request(io_channel);
    add_to_lamport_queue(io_channel, get_lamport_time(), io_channel->id);

    // waiting for responses from other processes

    Message *msg = (Message *)malloc(sizeof(Message));
    uint8_t processes_we_wait_for_reply = io_channel->children_num - 1; //children except for current
    uint8_t processes_still_running = io_channel->children_num - 1;

    while ((processes_we_wait_for_reply) || (io_channel->local_queue[get_lowest_time_request_num(io_channel)].pid != io_channel->id))
    {
        //lamport_queue_dump(io_channel);
        //printf("PROC %d STILL REMAIN %d\n",io_channel->id, processes_we_wait_for_reply);

        local_id msg_from = receive_any(io_channel, msg);
        timestamp_t request_received = msg->s_header.s_local_time;
        set_max_lamport_time(msg->s_header.s_local_time);
        switch (msg->s_header.s_type)
        {
        case CS_REPLY:
            // another process received our cs request and added it to its local queue
            processes_we_wait_for_reply -= 1;
            break;
        case CS_REQUEST:
            // another process wants cs, so add it to our local queue
            add_to_lamport_queue(io_channel, request_received, msg_from);
            // send him reply that we successfully received the message and added to queue
            send_cs_reply_to(io_channel, msg_from);
            break;
        case CS_RELEASE:
            // another process leaving cs, so deleting his request from local queue
            pop_from_lamport_queue(io_channel);
            break;
        case DONE:
            // process finished, don't care anymore
            processes_still_running -= 1;    
        default:
            break;
        }
    }

    return processes_still_running;
}

void wait_for_done(io_channel_t* io_channel, int remaining_processes) 
{
    // waiting for responses from other processes

    Message *msg = (Message *)malloc(sizeof(Message));

    while (remaining_processes > 0)
    {
        local_id msg_from = receive_any(io_channel, msg);
        timestamp_t request_received = msg->s_header.s_local_time;
        set_max_lamport_time(msg->s_header.s_local_time);
        switch (msg->s_header.s_type)
        {
        case CS_REQUEST:
            add_to_lamport_queue(io_channel, request_received, msg_from);
            send_cs_reply_to(io_channel, msg_from);
            break;
        case CS_RELEASE:
            pop_from_lamport_queue(io_channel);
            break;
        case DONE:
            remaining_processes--;
            break;
        }
    }
}

int release_cs(const void *self)
{
    io_channel_t *io_channel = (io_channel_t *)self;
    pop_from_lamport_queue(io_channel);
    send_cs_release(io_channel);
    return 0;
}

void do_useful_job(io_channel_t *io_channel)
{
    char print_buff[MAX_PAYLOAD_LEN];
    for (uint8_t i = 1; i <= io_channel->id * 5; i++)
    {
        sprintf(print_buff, log_loop_operation_fmt, io_channel->id, i, io_channel->id * 5);
        print(print_buff);
    }
}
