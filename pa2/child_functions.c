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

void balance_init(io_channel_t *io_channel, balance_t balance)
{
    io_channel->balance_history = (BalanceHistory){
            .s_id = io_channel->id,
            .s_history_len = 1};

    io_channel->balance_history.s_history[0] = (BalanceState){
            .s_balance = balance,
            .s_time = 0,
            .s_balance_pending_in = 0};
}

void handle_stop_and_transfer(io_channel_t *io_channel, balance_t balance) {
    Message *msg = (Message*) malloc(sizeof(Message));
    int loopbreak = 0;
    while (!loopbreak) {
        if (receive_any(io_channel, msg) == 0) {
            timestamp_t timestamp = get_physical_time();
            for (timestamp_t t = io_channel->balance_history.s_history_len; t < timestamp; t++) {
                io_channel->balance_history.s_history[t] = (BalanceState) {
                        .s_balance = balance,
                        .s_time = t,
                        .s_balance_pending_in = 0};
                io_channel->balance_history.s_history_len++;
            }

            switch (msg->s_header.s_type) {
                case STOP: {
                    io_channel->balance_history.s_history[timestamp] = (BalanceState) {
                            .s_balance = balance,
                            .s_time = timestamp,
                            .s_balance_pending_in = 0};
                    io_channel->balance_history.s_history_len++;
                    loopbreak = 1;
                }
                case TRANSFER: {
                    TransferOrder *transfer_order = (TransferOrder *) msg->s_payload;
                    //printf("id - %d, src - %d, dst - %d\n", io_channel->id, transfer_order->s_src, transfer_order->s_dst);
                    if (transfer_order->s_dst == io_channel->id) {
                        balance += transfer_order->s_amount;

                        log_transfer_in(transfer_order);

                        msg->s_header.s_type = ACK;
                        msg->s_header.s_payload_len = 0;
                        send(io_channel, PARENT_ID, msg);
                    } else if (transfer_order->s_src == io_channel->id) {
                        balance -= transfer_order->s_amount;

                        log_transfer_out(transfer_order);			

                        send(io_channel, transfer_order->s_dst, msg);
                    }
		
                }
                default:
                    break;
            }
        }
    }
}

int send_history(io_channel_t *io_channel)
{
    Message *history_message = (Message*) malloc(sizeof(Message));
    history_message->s_header.s_type = BALANCE_HISTORY;
    history_message->s_header.s_payload_len = io_channel->balance_history.s_history_len * sizeof(BalanceHistory);
    *((BalanceHistory *)history_message->s_payload) = io_channel->balance_history;
    if (!send(io_channel, PARENT_ID, history_message))
    {
        return 1;
    }
    puts ("HIST_SENT");
    return 0;
}

int send_started(io_channel_t *io_channel, Message *started_message)
{
    sprintf(started_message->s_payload, log_started_fmt, get_physical_time(), io_channel->id, getpid(), getppid(),
            io_channel->balance_history.s_history->s_balance);
    started_message->s_header.s_payload_len = (uint16_t)strlen(started_message->s_payload);
    if (!send_multicast(io_channel, started_message))
    {
        return 1;
    }
    return 0;
}

int send_done(io_channel_t *io_channel, Message *done_message)
{
    sprintf(done_message->s_payload, log_done_fmt, get_physical_time(), io_channel->id,
            io_channel->balance_history.s_history->s_balance);
    done_message->s_header.s_payload_len = (uint16_t)strlen(done_message->s_payload);
    if (!send_multicast(io_channel, done_message))
    {
        return 1;
    }
    return 0;
}

