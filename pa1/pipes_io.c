//
// Created by rogo on 24.03.20.
//

#include <stdlib.h>
#include <stdio.h>
#include <zconf.h>

#include "pipes_io.h"

pipe_fd_t*** create_pipes(uint8_t sum_process_num) {
    pipe_fd_t*** pipes_array = (pipe_fd_t***) calloc(sum_process_num, sizeof(pipe_fd_t**));
    for (uint8_t src = 0; src < sum_process_num; src++) {
        pipes_array[src] = (pipe_fd_t**) calloc(sum_process_num, sizeof(pipe_fd_t*));
        for (uint8_t dst = 0; dst < sum_process_num; dst++) {
            if (src!=dst) {
               //printf("%d + %d\n", src, dst); - just for debug
                int pipe_fd[2];
                if (pipe(pipe_fd) < 0) {
                    fprintf(stderr,"ERROR: cannot create channel between process %d and process %d",src,dst);
                    exit(1);
                }
                pipe_fd_t* channel_fd = (pipe_fd_t*) calloc(1,sizeof(pipe_fd_t));
                channel_fd->read_fd = pipe_fd[0];
                channel_fd->write_fd = pipe_fd[1];
                pipes_array[src][dst] = channel_fd;
            }
        }
    }
    return pipes_array;
}

