#include "lamport.h"
#include "ipc.h"
#include "banking.h"

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
