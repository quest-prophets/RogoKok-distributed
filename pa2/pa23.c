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



}
