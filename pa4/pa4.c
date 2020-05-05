#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <assert.h>

#include "util.h"
#include "child_functions.h"
#include "pipes_io.h"
#include "ipc.h"
#include "log.h"
#include "banking.h"
#include "lamport.h"

int main(int argc, char const *argv[])
{

     // variables

    uint8_t children_num;
    uint8_t sum_process_num;

    // input validation

    if ((argc != 3) || (strcmp(argv[1], "-p") != 0))
    {
        fprintf(stderr, "USAGE: pa1 -p <number of children>");
        return 1;
    }
    else
    {
        children_num = parse_arg(argv[2]);
        if (children_num > MAX_CHILDREN_NUM)
        {
            fprintf(stderr, "ERROR: maximum children number is %d", MAX_CHILDREN_NUM);
            return 1;
        }
        sum_process_num = children_num + (uint8_t) 1;
    }

    // logs init
    log_init();

    // creating read/write channels
    io_channel_t *io_channel = create_pipes(sum_process_num);
    activate_nonblock(io_channel);

    // creating array of running processes
    pid_t processes[sum_process_num];
    // forking children
    processes[0] = getpid();
    int loopbreak = 0;
    for (uint8_t pid = 1; pid <= children_num && !loopbreak; pid++)
    {
        int child_process = fork();
        switch (child_process)
        {
        case -1:
            // error
            fprintf(stderr, "ERROR: fork failed\n");
            return 1;
        case 0:
            // child process
            io_channel->id = pid;
            close_unused_pipes(io_channel);
            loopbreak = 1;
            break;
        default:
            // parent process
            io_channel->id = PARENT_ID;
            processes[pid] = child_process;
            break;
        }
    }

    close_unused_pipes(io_channel);

    if (io_channel->id == PARENT_ID)
    {
        // parent
        receive_from_all_processes(io_channel, 0); // receiving all STARTED
        log_received_all_started(io_channel);
        // here will be process job 
        receive_from_all_processes(io_channel, 1); // receiving all DONE
        log_received_all_done(io_channel);
        
        // waiting for processes to finish
        for (uint8_t pid = 1; pid <= io_channel->children_num; pid++)
            if (!waitpid(processes[pid], NULL, 0))
            {
                return 1;
            }
    }
    else
    {
        // child
        send_started(io_channel);                  // send to all - STARTED
        log_started(io_channel);
        receive_from_all_processes(io_channel, 0);                     // receiving all STARTED
        log_received_all_started(io_channel);
        // here will be process job
        send_done(io_channel);                        // send to all - DONE
        log_done(io_channel);
        receive_from_all_processes(io_channel, 1);                     // receiving all DONE
        log_received_all_done(io_channel);
    }

    log_events_close();
    log_pipes_close();

    return 0;
}
