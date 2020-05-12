
#include "util.h"
#include "pipes_io.h"
#include "ipc.h"

void init_dr_array(io_channel_t *io_channel) 
{
    for (uint8_t i = 0; i < MAX_PROCESSES_NUM; i++)
    {
        io_channel->dr_array[i] = 0;
    }
}
