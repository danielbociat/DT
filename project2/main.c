#include <stdio.h>
#include "parser.h"

int main(int argc, char *argv[]){
    FILE* file = fopen(argv[1], "r");
    char chr;
    STATE_MACHINE_RETURN_VALUE ret;

    at_command_reset();
    while(fscanf(file, "%c", &chr) > 0){
        ret = at_command_parser(chr);

        switch(ret){
            case STATE_MACHINE_NOT_READY:
                break;
            case STATE_MACHINE_READY_OK:
                at_command_print();
                at_command_reset();
                break;
            case STATE_MACHINE_READY_WITH_ERROR:
                at_command_reset();
                break;
        }

        if(ret == STATE_MACHINE_READY_WITH_ERROR)
            break;
    }

    if(ret == STATE_MACHINE_NOT_READY){
        printf("Incomplete transmission\n");
    }

    return 0;
}
