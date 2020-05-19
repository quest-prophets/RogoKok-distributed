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
#include "util.h"//

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

int send_cs_request(io_channel_t *io_channel, local_id send_to)
{
    inc_lamport_time();
    Message *cs_request_message = create_timed_message(MESSAGE_MAGIC, CS_REQUEST, 0, get_lamport_time());
    sprintf(cs_request_message->s_payload, log_done_fmt, get_lamport_time(), io_channel->id,
             io_channel->balance_history.s_history->s_balance);
    cs_request_message->s_header.s_payload_len = (uint16_t)strlen(cs_request_message->s_payload);
    if (!send(io_channel, send_to, cs_request_message))
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


// pa6 functions for dining philosophers algorithm

int request_cs(const void *self)
{
    io_channel_t *io_channel = (io_channel_t *)self;
    uint8_t processes_still_running = io_channel->children_num-1;

    Message *msg = (Message *)malloc(sizeof(Message));

    while (1)
    {

        for (local_id pid = 1; pid <= io_channel->children_num; pid++) 
        {
            if (pid != io_channel->id)
            {
                if (io_channel->philosophers_dinner.reqf[pid] && !io_channel->philosophers_dinner.fork[pid])
                {
                    io_channel->philosophers_dinner.reqf[pid] = 0;
                    send_cs_request(io_channel, pid);
                }
            }
        }

        local_id msg_from = receive_any(io_channel, msg);
        if (msg_from != -1)
         {
            set_max_lamport_time(msg->s_header.s_local_time);

            switch (msg->s_header.s_type)
            {
                case CS_REPLY:
                    io_channel->philosophers_dinner.dirty[msg_from] = 0;
                    io_channel->philosophers_dinner.fork[msg_from] = 1;
                    break;
                case CS_REQUEST:
                    if (io_channel->philosophers_dinner.fork[msg_from] && io_channel->philosophers_dinner.dirty[msg_from])
                    {
                        io_channel->philosophers_dinner.fork[msg_from] = 0;
                        send_cs_reply_to(io_channel, msg_from);
                    }
                    io_channel->philosophers_dinner.reqf[msg_from] = 1;
                    break;
                case DONE:
                    processes_still_running--;
                default:
                    break;
            }
        }

        if (has_all_necessary_forks(io_channel))
            break;
    }

    free(msg);
    return processes_still_running;
}

void wait_for_done(io_channel_t* io_channel, int remaining_processes) 
{
    // waiting for responses from other processes

    Message *msg = (Message *)malloc(sizeof(Message));

    while (remaining_processes > 0)
    {
        local_id msg_from = receive_any(io_channel, msg);
        if (msg_from == -1)
            continue;
        set_max_lamport_time(msg->s_header.s_local_time);
        switch (msg->s_header.s_type)
        {
        case CS_REQUEST:
            send_cs_reply_to(io_channel, msg_from);
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
    for (local_id pid = 1; pid <= io_channel->children_num; pid++)
    {
        if (pid == io_channel->id) 
            continue;
        if (io_channel->philosophers_dinner.reqf[pid])
        {
            io_channel->philosophers_dinner.fork[pid] = 0;
            send_cs_reply_to(io_channel, pid);
        }
        io_channel->philosophers_dinner.dirty[pid] = 1;
    }
    return 0;
}

void do_useful_job(io_channel_t *io_channel)
{
    char print_buff[128];
    for (uint8_t i = 1; i <= io_channel->id * 5; i++) {
    sprintf(print_buff, log_loop_operation_fmt, io_channel->id, i, io_channel->id * 5);
    print(print_buff);
    }
}
