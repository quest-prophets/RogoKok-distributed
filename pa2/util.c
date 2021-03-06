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

int blocking_receive(void *self, local_id from, Message *msg)
{
    io_channel_t *io_channel = (io_channel_t *)self;

    if (from > io_channel->children_num) {
        return 1;
    }

    ssize_t read_header = read(io_channel->io_channels[from][io_channel->id].read_fd, &msg->s_header, sizeof(MessageHeader));

    if (msg->s_header.s_magic != MESSAGE_MAGIC)
    {
        return 2;
    }

    ssize_t read_payload = read(io_channel->io_channels[from][io_channel->id].read_fd, &msg->s_payload, msg->s_header.s_payload_len);
    if (read_header < sizeof(MessageHeader) || read_payload < msg->s_header.s_payload_len)
    {
        return 3;
    }
    // printf("Процесс %d получил сообщение %d от процесса %d\n", io_channel->id, msg->s_header.s_type, from);
    return 0;
}

int receive_from_all_processes(io_channel_t* io_channel, int16_t type)
{
    for (uint8_t pid = 1; pid <= io_channel->children_num; pid++) {
        Message message;
        if (pid != io_channel->id) {
			while (1) {
            receive(io_channel, pid, &message);
            if (message.s_header.s_type==type) 
                break;
        }
		//if (message.s_header.s_type!=type) printf("BAN - %d received type %d, from %d\n", io_channel->id, message.s_header.s_type, pid);
        }
    }

    return 0;
}
