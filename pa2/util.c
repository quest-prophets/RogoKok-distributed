#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>

#include "util.h"
#include "ipc.h"
#include "pipes_io.h"
#include "pa2345.h"

uint8_t parse_arg (const char* arg) {
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

Message *create_timed_message(uint16_t magic, int16_t type, uint16_t payload_len, timestamp_t local_time)
{
    Message* msg = create_message(magic, type);
    msg->s_header.s_payload_len = payload_len;
    msg->s_header.s_local_time = local_time;
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
