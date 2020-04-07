#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "pipes_io.h"
#include "log.h"

io_channel_t *create_pipes(uint8_t sum_process_num)
{
    io_channel_t *pipes_struct = (io_channel_t *)calloc(1, sizeof(io_channel_t));
    for (uint8_t src = 0; src < sum_process_num; src++)
    {
        for (uint8_t dst = 0; dst < sum_process_num; dst++)
        {
            if (src != dst)
            {
                int pipe_fd[2];
                if (pipe(pipe_fd) < 0)
                {
                    fprintf(stderr, "ERROR: cannot create channel between process %d and process %d", src, dst);
                    exit(1);
                }
                pipe_fd_t *channel_fd = (pipe_fd_t *)calloc(1, sizeof(pipe_fd_t));
                pipes_struct->children_num = sum_process_num - (uint8_t)1;
                channel_fd->read_fd = pipe_fd[0];
                channel_fd->write_fd = pipe_fd[1];
                pipes_struct->io_channels[src][dst] = *channel_fd;
                log_pipe_open(src, dst);
            }
        }
    }
    return pipes_struct;
}

void close_unused_pipes(io_channel_t *pipes_struct)
{
    for (uint8_t src = 0; src <= pipes_struct->children_num; src++)
    {
        for (uint8_t dst = 0; dst <= pipes_struct->children_num; dst++)
        {
            if ((src != pipes_struct->id && dst != pipes_struct->id && src != dst))
            {
                close(pipes_struct->io_channels[src][dst].read_fd);
                close(pipes_struct->io_channels[src][dst].write_fd);
            }
            if (src != pipes_struct->id && dst == pipes_struct->id)
            {
                close(pipes_struct->io_channels[src][dst].write_fd);
            }
            if (src == pipes_struct->id && dst != pipes_struct->id)
            {
                close(pipes_struct->io_channels[src][dst].read_fd);
            }
        }
    }
}

