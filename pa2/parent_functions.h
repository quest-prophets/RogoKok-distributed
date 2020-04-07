#ifndef PA1TEST_PARENT_FUNCTIONS_H
#define PA1TEST_PARENT_FUNCTIONS_H

#include "ipc.h"
#include "pipes_io.h"
#include "banking.h"

int send_stop(io_channel_t* io_channel, Message* stop_message);

int receive_history_from_all(io_channel_t* io_channel, AllHistory* banking_history);

#endif //PA1TEST_PARENT_FUNCTIONS_H
