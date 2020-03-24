#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "util.h"

int main(int argc, char const *argv[]) {

    uint8_t num_children;

    if ((argc!=3) || (strcmp(argv[1],"-p")!=0)) {
        fprintf(stderr, "USAGE: pa1 -p <number of children>");
        return 1;
    } else {
         num_children = parse_children_num(argv[2]);
         if (num_children > 9) {    //TODO: replace with constant
             fprintf(stderr, "ERROR: maximum children number is 9");
             return 1;
         }
    }

    return 0;
}

