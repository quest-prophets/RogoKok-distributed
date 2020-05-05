#ifndef PA1TEST_CHILD_FUNCTIONS_H
#define PA1TEST_CHILD_FUNCTIONS_H

#include "ipc.h"
#include "pipes_io.h"
#include "banking.h"

int send_started(io_channel_t* io_channel);

int send_done(io_channel_t* io_channel);

void do_useful_job(io_channel_t *io_channel);

int send_cs_request(io_channel_t *io_channel);

int send_cs_release(io_channel_t *io_channel);

int send_cs_reply_to(io_channel_t *io_channel, local_id reply_to);

#endif //PA1TEST_CHILD_FUNCTIONS_H
