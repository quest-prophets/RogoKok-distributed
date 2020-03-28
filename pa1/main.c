#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <zconf.h>
#include <wait.h>

#include "util.h"
#include "pipes_io.h"
#include "ipc.h"
#include "pa1.h"

int main(int argc, char const *argv[])
{

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
        children_num = parse_children_num(argv[2]);
        if (children_num > MAX_CHILDREN_NUM)
        {
            fprintf(stderr, "ERROR: maximum children number is %d", MAX_CHILDREN_NUM);
            return 1;
        }
        sum_process_num = children_num + (uint8_t) 1;
    }

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
                process_id = pid;
                loopbreak = 1;
                break;
            default:
                // parent process
                process_id = PARENT_ID;
                processes[pid] = child_process;
                break;
        }
    }

    // creating read/write channels
    io_channel_t *io_channels = create_pipes(sum_process_num);
    io_channels->id = process_id;

    // sending STARTED message for children
    Message* started_message = create_message(MESSAGE_MAGIC, STARTED);
    if (process_id == PARENT_ID)
    {
        // parent
        sprintf(started_message->s_payload, log_started_fmt, process_id, getpid(), getppid());
        started_message->s_header.s_payload_len = (uint16_t) strlen(started_message->s_payload);
        send_multicast(io_channels, started_message);
    }

    // receiving messages from children
    for (uint8_t id = 1; id <= children_num; id++)
    {
        Message message_from_child;
        if (id == process_id) continue;
        receive(io_channels, id, &message_from_child);
    }

    // sending child work DONE for other processes
    Message* done_message = create_message(MESSAGE_MAGIC, DONE);
    if (process_id != PARENT_ID)
    {
        // child
        sprintf(started_message->s_payload, log_done_fmt, process_id);
        done_message->s_header.s_payload_len = (uint16_t) strlen(done_message->s_payload);
        send_multicast(io_channels, done_message);
    }

    // receiving messages from children
    for (uint8_t id = 1; id <= children_num; id++)
    {
        Message message_from_child;
        if (id == process_id) continue;
        receive(io_channels, id, &message_from_child);
    }

    // waiting for children to stop
    if (process_id == PARENT_ID)
    {
        // parent
        for (size_t i = 1; i <= children_num; i++)
            waitpid(processes[i], NULL, 0);
    }

    return 0;
}
