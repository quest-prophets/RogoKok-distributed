#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "parent_functions.h"
#include "pipes_io.h"
#include "pa2345.h"
#include "banking.h"
#include "util.h"
#include "lamport.h"

int send_stop(io_channel_t* io_channel, Message* stop_message)
{
    stop_message->s_header.s_payload_len = (uint16_t) strlen(stop_message->s_payload);
    inc_lamport_time();
    if (!send_multicast(io_channel, stop_message))
    {
        return 1;
    }
    return 0;
}

int receive_history_from_all(io_channel_t* io_channel, AllHistory* banking_history)
{
    Message* msg = (Message*)malloc(sizeof(Message));
    banking_history->s_history_len = io_channel->children_num;
    for (uint8_t pid = 1; pid <= io_channel->children_num; pid++) {
        if (pid != io_channel->id) {
            if (receive(io_channel, pid, msg) != 0 || msg->s_header.s_type != BALANCE_HISTORY)
            {
		        fprintf(stderr, "no BALANCE_HISTORY received\n");
                return 3;
            }
            BalanceHistory* balance_history = (BalanceHistory*) msg->s_payload;
            banking_history->s_history[pid-1] = *balance_history;
        }
    }
    free(msg);
    return 0;
}
