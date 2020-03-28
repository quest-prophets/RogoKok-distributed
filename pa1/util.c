#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zconf.h>
#include <wait.h>

#include "util.h"
#include "ipc.h"
#include "pipes_io.h"
#include "pa1.h"

uint8_t parse_children_num (const char* arg) {
    char *arg_end;
    uint8_t num;
    if (arg[0] == '-') {
        fprintf(stderr,"ERROR: argument must be a positive number");
        exit(1);
    }
    num = (uint8_t) strtoul(arg,&arg_end,10);
    if (*arg_end != '\0') {
        fprintf(stderr, "ERROR: argument must be a number");
        exit(1);
    }
    return num;
}


Message *create_message(uint16_t magic, int16_t type)
{
    Message* msg = (Message*) malloc(sizeof(Message));
    msg->s_header.s_magic = magic;
    msg->s_header.s_type = type;
    return msg;
}

int receive_from_all_processes(io_channel_t* io_channel)
{
    for (uint8_t pid = 1; pid <= io_channel->children_num; pid++) {
        Message message;
        if (pid != io_channel->id) {
            if (!receive(io_channel, pid, &message)){
                return 1;
            }
        }
    }
    return 0;
}

int send_started(io_channel_t* io_channel, Message* started_message)
{
    sprintf(started_message->s_payload, log_started_fmt, io_channel->id, getpid(), getppid());
    started_message->s_header.s_payload_len = (uint16_t) strlen(started_message->s_payload);
    if (!send_multicast(io_channel, started_message))
    {
        return 1;
    }
    return 0;
}

int send_done(io_channel_t* io_channel, Message* done_message)
{
    sprintf(done_message->s_payload, log_done_fmt, io_channel->id);
    done_message->s_header.s_payload_len = (uint16_t) strlen(done_message->s_payload);
    if (!send_multicast(io_channel, done_message))
    {
        return 1;
    }
    return 0;
}
