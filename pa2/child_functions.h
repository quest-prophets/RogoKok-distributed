#ifndef PA1TEST_CHILD_FUNCTIONS_H
#define PA1TEST_CHILD_FUNCTIONS_H

#include "ipc.h"
#include "pipes_io.h"
#include "banking.h"

void run_child(io_channel_t *io_channel, Message *msg, balance_t balance);

int send_started(io_channel_t* io_channel, Message* started_message);

int send_done(io_channel_t* io_channel, Message* done_message);

#endif //PA1TEST_CHILD_FUNCTIONS_H
