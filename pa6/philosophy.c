#include <stdio.h>
#include <stdlib.h>

#include "pipes_io.h"
#include "util.h"
#include "ipc.h"

void dinner_init(io_channel_t *io_channel)
{
    for (local_id pid = 1; pid <= io_channel->children_num; pid++)
    {
        if (pid == io_channel->id) 
            continue;

        if (pid < io_channel->id)
        {
            io_channel->philosophers_dinner.reqf[pid] = 0;
            io_channel->philosophers_dinner.fork[pid] = 1;
            io_channel->philosophers_dinner.dirty[pid] = 1;
        }
        else
        {
            io_channel->philosophers_dinner.reqf[pid] = 1;
            io_channel->philosophers_dinner.fork[pid] = 0;
            io_channel->philosophers_dinner.dirty[pid] = 0;
        }
    }
}

uint8_t has_all_necessary_forks(io_channel_t *io_channel)
{
    for (local_id pid = 1; pid <= io_channel->children_num; pid++)
    {
        if (pid == io_channel->id) 
            continue;
        
        if (!io_channel->philosophers_dinner.fork[pid] ||
            (io_channel->philosophers_dinner.dirty[pid] && io_channel->philosophers_dinner.reqf[pid])) {
                // printf("ERROR - curr_id: %d, pid: %d, fork: %d, dirty: %d, reqf: %d\n", io_channel->id, pid,
                //     io_channel->philosophers_dinner.fork[pid],
                //     io_channel->philosophers_dinner.dirty[pid],
                //     io_channel->philosophers_dinner.reqf[pid]);
                return 0;
            }
    }
    return 1;
}


