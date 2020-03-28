#include <stdio.h>
#include <stdlib.h>

#include "util.h"
#include "ipc.h"

uint8_t parse_children_num (const char* arg) {
    char *arg_end;
    uint8_t num;
    if (arg[0] == '-') {
        fprintf(stderr,"ERROR: argument must be a positive number");
        exit(1);
    }
    num = (uint8_t) strtoul(arg,&arg_end,10);
    if (*arg_end != '\0') {
        fprintf(stderr, "ERROR: argument must be a number");
        exit(1);
    }
    return num;
}

Message* create_message_by_type(MessageType type)
{
    Message* msg = (Message*) malloc(sizeof(Message));
    msg->s_header.s_magic = MESSAGE_MAGIC;
    msg->s_header.s_type = type;
    return msg;
}