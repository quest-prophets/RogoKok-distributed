#ifndef PA1_PIPES_IO_H
#define PA1_PIPES_IO_H

#include <stdint.h>

#include "ipc.h"
#include "banking.h"

#define MAX_CHILDREN_NUM 9
#define MAX_PROCESSES_NUM (MAX_CHILDREN_NUM + 1)

typedef struct
{
    int read_fd;
    int write_fd;
} __attribute__((packed)) pipe_fd_t;

typedef struct
{
    local_id id;
    uint8_t children_num;
    pipe_fd_t io_channels[MAX_PROCESSES_NUM][MAX_PROCESSES_NUM];
    BalanceHistory balance_history;
} __attribute__((packed)) io_channel_t;

io_channel_t *create_pipes(uint8_t process_num);

void close_unused_pipes(io_channel_t *pipes_struct);

void activate_nonblock(io_channel_t *io_channel);

#endif //PA1_IO_H

