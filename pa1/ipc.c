#include <stdio.h>

#include "ipc.h"
#include "pipes_io.h"

int send(void * self, local_id dst, const Message * msg)
{
    io_channel_t* io_channel = (io_channel_t*) self;

    if (dst >= io_channel->children_num) {
        return 1;
    }
    if (msg->s_header.s_magic != MESSAGE_MAGIC) {
        return 2;
    }

    size_t msg_len = sizeof(MessageHeader) + msg->s_header.s_payload_len;
    return write(io_channel->io_channels[io_channel->id][dst].write_fd, msg, msg_len) != msg_len;
}

int send_multicast(void * self, const Message * msg) 
{
    io_channel_t* io_channel = (io_channel_t*) self;

    for (local_id dst = 0; dst < io_channel->children_num; dst++) {
        if (dst != io_channel->id) {
            int result = send(self, dst, msg);
            if (result > 0) {
                return result;
            }
        }
    }
    return 0;
}

int receive(void * self, local_id from, Message * msg) 
{
    io_channel_t* io_channel = (io_channel_t*) self;

    if (from >= io_channel->children_num) {
        return 1;
    }
    if (msg->s_header.s_magic != MESSAGE_MAGIC) {
        return 2;
    }

    int read_header = read(io_channel->io_channels[from][io_channel->id], msg->s_header, sizeof(MessageHeader));
    int read_payload = read(io_channel->io_channels[from][io_channel->id], msg->s_payload, msg->s_header.s_payload_len);
    if (read_header < sizeof(MessageHeader) || read_payload < msg->s_header.s_payload_len) 
    {
        return 3;
    }

    return 0;
}

int receive_any(void * self, Message * msg) 
{
    io_channel_t* io_channel = (io_channel_t*) self;

    for (local_id from = 0; from < io_channel->children_num; from++) {
        if (from != io_channel->id) {
            int result = send(self, from, msg);
            if (result > 0) {
                return result;
            }
        }
    }
    return 0;
}
