#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "parent_functions.h"
#include "pipes_io.h"
#include "pa2345.h"

int send_stop(io_channel_t* io_channel, Message* stop_message)
{
    stop_message->s_header.s_payload_len = (uint16_t) strlen(stop_message->s_payload);
    if (!send_multicast(io_channel, stop_message))
    {
        return 1;
    }
    return 0;
}
