#include "lamport.h"
#include "ipc.h"
#include "banking.h"

timestamp_t lamport_scalar_time = 0;

timestamp_t get_lamport_time() 
{
    return lamport_scalar_time;
}

void inc_lamport_time()
{
    lamport_scalar_time++;
}

timestamp_t get_max_lamport_time(timestamp_t another_time)
{
    if (lamport_scalar_time < another_time)
    {
        lamport_scalar_time = another_time;
    }
    return lamport_scalar_time;
}
