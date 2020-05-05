#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ipc.h"
#include "pipes_io.h"

// const char* msgtype(const Message* msg) 
// {
//     static char* MESSAGE_TYPE_STR[] = {"STARTED", "DONE", "ACK", "STOP", "TRANSFER", "BALANCE_HISTORY"};
//     static size_t MESSAGE_TYPE_CNT = sizeof(MESSAGE_TYPE_STR) / sizeof(MESSAGE_TYPE_STR[0]);
//     return msg->s_header.s_type < MESSAGE_TYPE_CNT ? MESSAGE_TYPE_STR[msg->s_header.s_type] : "<unknown>";
// }

int send(void *self, local_id dst, const Message *msg)
{
    io_channel_t *io_channel = (io_channel_t *)self;

    if (dst > io_channel->children_num)
    {
        return 1;
    }
    if (msg->s_header.s_magic != MESSAGE_MAGIC)
    {
        return 2;
    }

    size_t msg_len = sizeof(MessageHeader) + msg->s_header.s_payload_len;
    // printf("process %d sent msg to %d, type is %s\n", io_channel->id, dst, msgtype(msg));
    return write(io_channel->io_channels[io_channel->id][dst].write_fd, msg, msg_len) != msg_len;
}

int send_multicast(void *self, const Message *msg)
{
    io_channel_t *io_channel = (io_channel_t *)self;

    for (local_id dst = 0; dst <= io_channel->children_num; dst++)
    {
        if (dst != io_channel->id)
        {
            int result = send(self, dst, msg);
            if (result > 0)
            {
                return result;
            }
        }
    }
    return 0;
}

int receive(void *self, local_id from, Message *msg)
{
    io_channel_t *io_channel = (io_channel_t *)self;

    if (from > io_channel->children_num)
    {
        return 1;
    }

    while (1)
    {
        ssize_t read_flag = read(io_channel->io_channels[from][io_channel->id].read_fd, &msg->s_header, sizeof(MessageHeader));
        if (read_flag == -1 || read_flag == 0)
        {
            continue;
        }
        if (msg->s_header.s_payload_len > 0)
        {
            do
            {
                read_flag = read(io_channel->io_channels[from][io_channel->id].read_fd, &msg->s_payload, msg->s_header.s_payload_len);
            } while (read_flag == -1 || read_flag == 0);
        }
        // printf("process %d received msg from %d, type is %s\n", io_channel->id, from, msgtype(msg));
        return 0;
    }
}

int receive_any(void *self, Message *msg)
{
    io_channel_t *io_channel = (io_channel_t *)self;
    ssize_t read_flag;

    while (1)
    {
        for (local_id from = 0; from <= io_channel->children_num; from++)
        {
            if (from == io_channel->id)
                continue;

            read_flag = read(io_channel->io_channels[from][io_channel->id].read_fd, &msg->s_header, sizeof(MessageHeader));

            if (read_flag == -1)
                continue;

            if (msg->s_header.s_payload_len > 0)
            {
                do
                {
                    read_flag = read(io_channel->io_channels[from][io_channel->id].read_fd, &msg->s_payload, msg->s_header.s_payload_len);
                } while (read_flag == -1);
            }
            return from;
        }
    }
}
