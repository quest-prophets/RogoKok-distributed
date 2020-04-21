#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ipc.h"
#include "pipes_io.h"

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
    //  printf("Процесс %d отправил сообщение %d процессу %d \n", io_channel->id, msg->s_header.s_type,dst);
	//printf("SEND!!!! MSG IS: %s; SEND!!! TYPE IS: %d\n", msg->s_payload, msg->s_header.s_type);
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
	//printf("MSG IS: %s; TYPE IS: %d\n", msg->s_payload, msg->s_header.s_type);
        return 0;
    }
}

int receive_any(void *self, Message *msg)
{
    io_channel_t *io_channel = (io_channel_t *)self;
    ssize_t read_flag;

    while(1) 
	{
        for (local_id from = 0; from <= io_channel->children_num; from++)
        {
            if (from == io_channel->id) continue;

            read_flag = read(io_channel->io_channels[from][io_channel->id].read_fd, &msg->s_header, sizeof(MessageHeader));

            if (read_flag == -1) continue;

            if (msg->s_header.s_payload_len > 0)
            {
                do
                {
                read_flag = read(io_channel->io_channels[from][io_channel->id].read_fd, &msg->s_payload, msg->s_header.s_payload_len);
                }
                while (read_flag == -1);
            }
            return 0;
        }
    }
}

