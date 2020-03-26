#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <zconf.h>

#include "util.h"
#include "pipes_io.h"

int main(int argc, char const *argv[]) {

    uint8_t children_num;
    uint8_t sum_process_num;

    // input validation

    if ((argc!=3) || (strcmp(argv[1],"-p")!=0)) {
        fprintf(stderr, "USAGE: pa1 -p <number of children>");
        return 1;
    } else {
         children_num = parse_children_num(argv[2]);
         if (children_num > MAX_CHILDREN_NUM) {
             fprintf(stderr, "ERROR: maximum children number is %d", MAX_CHILDREN_NUM);
             return 1;
         }
         sum_process_num = children_num + (uint8_t) 1;
    }

    // creating read/write channels
    io_channel_t* io_channels = create_pipes(sum_process_num);


    // forking children
    for (uint8_t pid = 1; pid <= children_num; pid++) {
        int child_process = fork();
        switch (child_process) {
            case -1:
                // error
                fprintf(stderr, "ERROR: fork failed\n");
                return 1;
            case 0:
                // child process

                break;
            default:
                // parent process
                break;
        }
    }

    return 0;
}
