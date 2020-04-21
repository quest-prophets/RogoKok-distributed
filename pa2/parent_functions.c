#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "parent_functions.h"
#include "pipes_io.h"
#include "pa2345.h"
#include "banking.h"

int send_stop(io_channel_t* io_channel, Message* stop_message)
{
    stop_message->s_header.s_payload_len = (uint16_t) strlen(stop_message->s_payload);
    if (!send_multicast(io_channel, stop_message))
    {
        return 1;
    }
    return 0;
}

int receive_history_from_all(io_channel_t* io_channel, AllHistory* banking_history)
{
    banking_history->s_history_len = io_channel->children_num;
    for (uint8_t pid = 1; pid <= io_channel->children_num; pid++) {
        Message message;
        if (pid != io_channel->id) {
            receive(io_channel, pid, &message);
            if (message.s_header.s_type != BALANCE_HISTORY)
            {
                return 3;
            }
            BalanceHistory* balance_history = (BalanceHistory*) message.s_payload;
            banking_history->s_history[pid-1] = *balance_history;
        }
    }
    return 0;
}
