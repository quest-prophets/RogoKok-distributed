//
// Created by rogo on 25.03.20.
//

#include "ipc.h"

int send(void * self, local_id dst, const Message * msg);


int send_multicast(void * self, const Message * msg);


int receive(void * self, local_id from, Message * msg);


int receive_any(void * self, Message * msg);


