#ifndef PA1TEST_CHILD_FUNCTIONS_H
#define PA1TEST_CHILD_FUNCTIONS_H

#include "ipc.h"
#include "pipes_io.h"
#include "banking.h"

int send_started(io_channel_t* io_channel, Message* started_message);

int send_done(io_channel_t* io_channel, Message* done_message);

#endif //PA1TEST_CHILD_FUNCTIONS_H
