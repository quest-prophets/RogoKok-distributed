//
// Created by rogo on 24.03.20.
//

#ifndef PA1_PIPES_IO_H
#define PA1_PIPES_IO_H


#include <stdint.h>

#include "ipc.h"

#define MAX_CHILDREN_NUM 9
#define MAX_PROCESSES_NUM (MAX_CHILDREN_NUM+1)

typedef struct {
    int read_fd;
    int write_fd;
} __attribute__((packed)) pipe_fd_t;

pipe_fd_t*** create_pipes(uint8_t process_num);

#endif //PA1_IO_H
