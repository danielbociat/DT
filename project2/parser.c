#include "parser.h"
#include <stdio.h>

STATE_MACHINE_RETURN_VALUE at_command_parser(uint8_t current_character){
    static uint8_t state;

    switch(state){
        case 0:
            if(current_character == '\r')
                state = 1;
            break;

        case 1:
            if(current_character == '\n')
                state = 2;
            else
                return STATE_MACHINE_READY_WITH_ERROR;
            break;

        case 2:
            if(current_character == 'O')
                state = 3;
            else{
                if (current_character == '+')
                    state = 14;
                else
                    return STATE_MACHINE_READY_WITH_ERROR;
            }
            break;

        case 3:

    }

    return STATE_MACHINE_NOT_READY;
}
