#ifndef PA1TEST_PARENT_FUNCTIONS_H
#define PA1TEST_PARENT_FUNCTIONS_H

#include "ipc.h"
#include "pipes_io.h"

int send_stop(io_channel_t* io_channel, Message* stop_message);

#endif //PA1TEST_PARENT_FUNCTIONS_H
