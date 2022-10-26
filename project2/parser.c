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
            switch(current_character){
                case 'O':
                    state = 3;
                    break;
                case '+':
                    state = 14;
                    break;
                case 'E':
                    state = 7;
                    break;
                default:
                    return STATE_MACHINE_READY_WITH_ERROR;
            }
            break;

        case 3:
            if(current_character == 'K')
                state = 4;
            else
                return STATE_MACHINE_READY_WITH_ERROR;
            break;

        case 4:
            if(current_character == '\r')
                state = 5;
            else
                return STATE_MACHINE_READY_WITH_ERROR;
            break;

        case 5:
            if(current_character == '\n'){
                state = 0; // Command is done, return to start
                return STATE_MACHINE_READY_OK;
            }
            else
                return STATE_MACHINE_READY_WITH_ERROR;
            break;

        case 7:
            if(current_character == 'R')
                state = 8;
            else
                return STATE_MACHINE_READY_WITH_ERROR;
            break;

        case 8:
            if(current_character == 'R')
                state = 9;
            else
                return STATE_MACHINE_READY_WITH_ERROR;
            break;

        case 9:
            if(current_character == 'O')
                state = 10;
            else
                return STATE_MACHINE_READY_WITH_ERROR;
            break;

        case 10:
            if(current_character == 'R')
                state = 11;
            else
                return STATE_MACHINE_READY_WITH_ERROR;
            break;

        case 11:
            if(current_character == '\r')
                state = 12;
            else
                return STATE_MACHINE_READY_WITH_ERROR;
            break;

        case 12:
            if(current_character == '\n')
                state = 0;
            return STATE_MACHINE_READY_WITH_ERROR; /// it is an error either way
            break;

        case 14:
            if( 32 <= (int)current_character &&  (int)current_character <= 128)
                state = 14;
            else{
                if(current_character == '\r')
                    state = 15;
                else
                    return STATE_MACHINE_READY_WITH_ERROR;
            }
            break;

        case 15:
            if(current_character == '\n')
                state = 16;
            else
                return STATE_MACHINE_READY_WITH_ERROR;
            break;

        case 16:
            switch(current_character){
                case '+':
                    state = 14;
                    break;
                case '\r':
                    state = 17;
                    break;
                default:
                    return STATE_MACHINE_READY_WITH_ERROR;
            }
            break;

        case 17:
            if(current_character == '\n')
                state = 18;
            else
                return STATE_MACHINE_READY_WITH_ERROR;
            break;

        case 18:
            switch(current_character){
                case 'O':
                    state = 3;
                    break;
                case 'E':
                    state = 7;
                    break;
                default:
                    return STATE_MACHINE_READY_WITH_ERROR;
            }
            break;
    }

    return STATE_MACHINE_NOT_READY;
}

void at_command_print(AT_COMMAND_DATA mydata){
    for(int i = 0; i < mydata.line_count; ++i){
        printf("%s", mydata.data[i]);
    }

    if(mydata.ok == 1)
        printf("Status: OK\n");
    else
        printf("Status: ERROR\n");
}

void at_command_reset(){
    memset(mydata.data, 0, sizeof(mydata));
    mydata.ok = 2;
}
