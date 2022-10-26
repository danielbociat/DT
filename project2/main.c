#include <stdio.h>
#include "parser.h"

int main(int argc, char *argv[]){
    FILE* file = fopen(argv[1], "r");
    char chr;
    STATE_MACHINE_RETURN_VALUE ret;

    while(fscanf(file, "%c", &chr) > 0){
        ret = at_command_parser(chr);
        //printf("%d", (int)chr);

        switch(ret){
            case STATE_MACHINE_NOT_READY:
                mydata.ok = 2;
                printf("STATE_MACHINE_NOT_READY\n");
                break;
            case STATE_MACHINE_READY_OK:
                mydata.ok = 1;
                at_command_reset();
                printf("STATE_MACHINE_READY_OK\n");
                break;
            case STATE_MACHINE_READY_WITH_ERROR:
                mydata.ok = 0;
                at_command_reset();
                printf("STATE_MACHINE_READY_WITH_ERROR\n");
                break;
        }
    }

    return 0;
}
