#include <stdlib.h>
#include <stdio.h>
#include <zconf.h>

#include "pipes_io.h"

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
                channel_fd->read_fd = pipe_fd[0];
                channel_fd->write_fd = pipe_fd[1];
                pipes_struct->io_channels[src][dst] = *channel_fd;
            }
        }
    }
    return pipes_struct;
}
