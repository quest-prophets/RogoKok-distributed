#ifndef PA1TEST_CHILD_FUNCTIONS_H
#define PA1TEST_CHILD_FUNCTIONS_H

#include "ipc.h"
#include "pipes_io.h"
#include "banking.h"

int send_started(io_channel_t* io_channel);

int send_done(io_channel_t* io_channel);

void balance_init(io_channel_t *io_channel, balance_t balance);

void handle_stop_and_transfer(io_channel_t *io_channel, balance_t balance);

int send_history(io_channel_t *io_channel);

#endif //PA1TEST_CHILD_FUNCTIONS_H
