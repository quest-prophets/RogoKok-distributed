#include <stdio.h>

#include "lamport.h"
#include "ipc.h"
#include "banking.h"
#include "pipes_io.h"
#include "util.h"

timestamp_t lamport_scalar_time = 0;

// just getting time
timestamp_t get_lamport_time() 
{
    return lamport_scalar_time;
}

// before send
// time = time + 1
// send (msg, type)
void inc_lamport_time()
{
    lamport_scalar_time++;
}

// after receive
// (msg, time_stamp) = receive
// time = max(time_stamp, time) + 1
void set_max_lamport_time(timestamp_t another_time)
{
    if (lamport_scalar_time < another_time)
    {
        lamport_scalar_time = another_time;
    }
    inc_lamport_time();
}

// adding cs_request info to local queue
void add_to_lamport_queue(io_channel_t *io_channel, timestamp_t time, local_id pid) 
{
    io_channel->local_queue[io_channel->last_record_num] = (lamport_queue_record_t) 
    {
        .request_time = time,
        .pid = pid
    };
    io_channel->last_record_num++;
}

// finding the first cs request (with lowest time mark)
int get_lowest_time_request_num(io_channel_t *io_channel)
{
    uint8_t min_time_num = 0;
    for (uint8_t record_num = 0; record_num < io_channel->last_record_num; record_num++)
    {
        if (io_channel->local_queue[record_num].request_time < io_channel->local_queue[min_time_num].request_time)
        {
            min_time_num = record_num;
        } else if (io_channel->local_queue[record_num].request_time == io_channel->local_queue[min_time_num].request_time)
        {
            if (io_channel->local_queue[record_num].pid < io_channel->local_queue[min_time_num].pid) 
            {
                min_time_num = record_num;
            }
        }
    }
    return min_time_num;  
}

// printing the whole queue for debugging
void lamport_queue_dump(io_channel_t *io_channel)
{
    for (uint8_t i = 0; i < io_channel->last_record_num; i++) 
    {
        printf("PROCESS %d index %d: pid is %d, time is %d\n", io_channel->id,
         i, io_channel->local_queue[i].pid, io_channel->local_queue[i].request_time);
    }
}

// deleting top element from local queue удаляем НОЛЕВОЙ элемент
void pop_from_lamport_queue(io_channel_t *io_channel)
{
    int top_element = get_lowest_time_request_num(io_channel);
    io_channel->local_queue[top_element] = io_channel->local_queue[io_channel->last_record_num-1];
    io_channel->last_record_num--;
}
