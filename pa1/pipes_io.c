//
// Created by rogo on 24.03.20.
//

#include <stdlib.h>
#include <stdio.h>
#include <zconf.h>

#include "pipes_io.h"

int** create_pipes(uint8_t sum_process_num) {
    int** pipes_array = (int**) malloc(sizeof(int*) * sum_process_num);
    for (uint8_t src = 0; src < sum_process_num; src++) {
        for (uint8_t dst = 0; dst < sum_process_num; dst++) {
            if (src!=dst) {
                pipes_array[src] = (int*) malloc(sizeof(int) * 2);
                int pipe_fd[2];
                if (pipe(pipe_fd) < 0) {
                    fprintf(stderr,"ERROR: cannot create channel between process %d and process %d",src,dst);
                    exit(1);
                }
                pipes_array[src] = pipe_fd;
            }
        }
    }
    return pipes_array;
}

