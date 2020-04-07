#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wait.h>
#include <unistd.h>
#include <sys/types.h>

#include "util.h"
#include "pipes_io.h"
#include "ipc.h"
#include "log.h"
#include "banking.h"

void transfer(void * parent_data, local_id src, local_id dst,
              balance_t amount)
{
    // student, please implement me - NO!
}

int main(int argc, char const *argv[])
{

    // variables

    uint8_t children_num;
    uint8_t sum_process_num;
    balance_t branch_balances[argc-2];

    // input validation

    if ((argc <= 3) || (strcmp(argv[1], "-p") != 0))
    {
        fprintf(stderr, "USAGE: pa2 -p <number of children>  <children balances>");
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
        if ((children_num + 3) != argc)
        {
            fprintf(stderr, "ERROR: expected %d balances after -p %s", children_num, argv[2]);
            return 1;
        }
        for (uint8_t i = 0; i < children_num; i++) {
            branch_balances[i+1] = parse_arg(argv[3+i]);
        }
        sum_process_num = children_num + (uint8_t) 1;
    }

    // logs init
    log_init();

    // creating read/write channels
    io_channel_t *io_channel = create_pipes(sum_process_num);

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

    Message* started_message = create_message(MESSAGE_MAGIC, STARTED);
    Message* done_message = create_message(MESSAGE_MAGIC, DONE);

    if (io_channel->id == PARENT_ID)
    {
        // parent
        receive_from_all_processes(io_channel); // receiving all STARTED
        log_received_all_started(io_channel);
        receive_from_all_processes(io_channel); // receiving all DONE
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
        send_started(io_channel,started_message); // send to all - STARTED
        log_started(io_channel);
        receive_from_all_processes(io_channel); // receiving all STARTED
        log_received_all_started(io_channel);
        send_done(io_channel, done_message);   // send to all - DONE
        log_done(io_channel);
        receive_from_all_processes(io_channel); // receiving all DONE
        log_received_all_done(io_channel);

    }

    log_events_close();
    log_pipes_close();

    return 0;

}

