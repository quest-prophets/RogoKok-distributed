#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <assert.h>

#include "util.h"
#include "child_functions.h"
#include "parent_functions.h"
#include "pipes_io.h"
#include "ipc.h"
#include "log.h"
#include "banking.h"

void transfer(void *parent_data, local_id src, local_id dst,
              balance_t amount)
{
    io_channel_t *io_channel = (io_channel_t *)parent_data;
    Message *msg = create_timed_message(MESSAGE_MAGIC, TRANSFER, sizeof(TransferOrder), get_physical_time());
    *((TransferOrder *)msg->s_payload) =
        (TransferOrder){
            .s_src = src, .s_dst = dst, .s_amount = amount};

    send(io_channel, src, msg);
    receive(io_channel, dst, msg);
    assert(msg->s_header.s_type == ACK);
}

int main(int argc, char const *argv[])
{

    // variables

    uint8_t children_num;
    uint8_t sum_process_num;
    balance_t branch_balances[argc - 2];
    AllHistory banking_history;

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
        for (uint8_t i = 0; i < children_num; i++)
        {
            branch_balances[i + 1] = parse_arg(argv[3 + i]);
        }
        sum_process_num = children_num + (uint8_t)1;
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

    Message *started_message = create_message(MESSAGE_MAGIC, STARTED);
    Message *done_message = create_message(MESSAGE_MAGIC, DONE);
    Message *stop_message = create_timed_message(MESSAGE_MAGIC, STOP, 0, get_physical_time());

    if (io_channel->id == PARENT_ID)
    {
        // parent
        receive_from_all_processes(io_channel); // receiving all STARTED
        log_received_all_started(io_channel);
        bank_robbery(io_channel, children_num);  // bank robbery
        send_stop(io_channel, stop_message);    // sending STOP to all
        receive_from_all_processes(io_channel); // receiving all DONE
        log_received_all_done(io_channel);
        receive_history_from_all(io_channel,&banking_history);
        print_history(&banking_history);        // printing all history
    }
    else
    {
        // child
        balance_init(io_channel, branch_balances[io_channel->id]);  // initial branch balances info
        send_started(io_channel, started_message);                  // send to all - STARTED
        log_started(io_channel);
        receive_from_all_processes(io_channel);                     // receiving all STARTED
        log_received_all_started(io_channel);
        // loop for TRANSFER and STOP waiting and handing
        handle_stop_and_transfer(io_channel, branch_balances[io_channel->id]);
        send_done(io_channel, done_message);                        // send to all - DONE
        log_done(io_channel);
        receive_from_all_processes(io_channel);                     // receiving all DONE
        log_received_all_done(io_channel);
        // sending history to parent
        send_history(io_channel);
    }

    log_events_close();
    log_pipes_close();

    return 0;
}
