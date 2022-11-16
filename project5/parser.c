#include "parser.h"
#include <stdio.h>
#include <string.h>

AT_COMMAND_DATA mydata;

STATE_MACHINE_RETURN_VALUE at_command_parser(uint8_t current_character, uint8_t isException){
    static uint8_t state = 0;
    static uint8_t curr_col = 0;
    switch(state){
        case 0:
            mydata.line_count = 0;
            if(current_character == '\r')
                state = 1;
            break;

        case 1:
            if(current_character == '\n')
                state = 2;
            else{
                printf("Received: %c\n", current_character);
                printf("Expected: <LF>\n");
                printf("Stopped in state: %d\n", state);

                state = 0;
                curr_col = 0;

                return STATE_MACHINE_READY_WITH_ERROR;
            }
            break;

        case 2:
            if(isException){
                curr_col = 0;
                if(mydata.line_count <= AT_COMMAND_MAX_LINES && curr_col < AT_COMMAND_MAX_LINE_SIZE){
                    mydata.line_count++;
                    mydata.data[mydata.line_count-1][curr_col] = current_character;
                    curr_col++;
                }

                state = 14;
                break;
            }

            switch(current_character){
                case 'O':
                    state = 3;
                    break;
                case '+':
                    curr_col = 0;
                    if(mydata.line_count <= AT_COMMAND_MAX_LINES && curr_col < AT_COMMAND_MAX_LINE_SIZE){
                        mydata.line_count++;
                        mydata.data[mydata.line_count-1][curr_col] = current_character;
                        curr_col++;
                    }

                    state = 14;
                    break;
                case 'E':
                    state = 7;
                    break;
                default:
                    printf("Received: %c\n", current_character);
                    printf("Expected: O, + or E\n");
                    printf("Stopped in state: %d\n", state);

                    state = 0;
                    curr_col = 0;

                    return STATE_MACHINE_READY_WITH_ERROR;
            }
            break;

        case 3:
            if(current_character == 'K')
                state = 4;
            else{
                printf("Received: %c\n", current_character);
                printf("Expected: K\n");
                printf("Stopped in state: %d\n", state);

                state = 0;
                curr_col = 0;

                return STATE_MACHINE_READY_WITH_ERROR;
            }
            break;

        case 4:
            if(current_character == '\r')
                state = 5;
            else{
                printf("Received: %c\n", current_character);
                printf("Expected: <CR>\n");
                printf("Stopped in state: %d\n", state);

                state = 0;
                curr_col = 0;

                return STATE_MACHINE_READY_WITH_ERROR;
            }
            break;

        case 5:
            if(current_character == '\n'){
                state = 0;
                curr_col = 0;
                mydata.ok = 1;
                return STATE_MACHINE_READY_OK;
            }
            else{
                printf("Received: %c\n", current_character);
                printf("Expected: <LF>\n");
                printf("Stopped in state: %d\n", state);

                state = 0;
                curr_col = 0;

                return STATE_MACHINE_READY_WITH_ERROR;
            }
            break;

        case 7:
            if(current_character == 'R')
                state = 8;
            else{
                printf("Received: %c\n", current_character);
                printf("Expected: R\n");
                printf("Stopped in state: %d\n", state);

                state = 0;
                curr_col = 0;
                return STATE_MACHINE_READY_WITH_ERROR;
            }
            break;

        case 8:
            if(current_character == 'R')
                state = 9;
            else{
                printf("Received: %c\n", current_character);
                printf("Expected: R\n");
                printf("Stopped in state: %d\n", state);
                state = 0;
                curr_col = 0;
                return STATE_MACHINE_READY_WITH_ERROR;
            }
            break;

        case 9:
            if(current_character == 'O')
                state = 10;
            else{
                printf("Received: %c\n", current_character);
                printf("Expected: O\n");
                printf("Stopped in state: %d\n", state);
                state = 0;
                curr_col = 0;
                return STATE_MACHINE_READY_WITH_ERROR;
            }
            break;

        case 10:
            if(current_character == 'R')
                state = 11;
            else{
                printf("Received: %c\n", current_character);
                printf("Expected: R\n");
                printf("Stopped in state: %d\n", state);
                state = 0;
                curr_col = 0;
                return STATE_MACHINE_READY_WITH_ERROR;
            }
            break;

        case 11:
            if(current_character == '\r')
                state = 12;
            else{
                printf("Received: %c\n", current_character);
                printf("Expected: <CR>\n");
                printf("Stopped in state: %d\n", state);
                state = 0;
                curr_col = 0;
                return STATE_MACHINE_READY_WITH_ERROR;
            }
            break;

        case 12:
            if(current_character == '\n'){
                state = 0;
                curr_col = 0;
                mydata.ok = 0;
                return STATE_MACHINE_READY_OK;
            }
            else{
                printf("Received: %c\n", current_character);
                printf("Expected: <LF>\n");
                printf("Stopped in state: %d\n", state);
                state = 0;
                curr_col = 0;
                return STATE_MACHINE_READY_WITH_ERROR;
            }
            break;

        case 14:
            if( 32 <= (int)current_character &&  (int)current_character <= 128){
                if(mydata.line_count <= AT_COMMAND_MAX_LINES && curr_col < AT_COMMAND_MAX_LINE_SIZE){
                    mydata.data[mydata.line_count-1][curr_col] = current_character;
                    curr_col++;
                }
                state = 14;
            }

            else{
                if(current_character == '\r'){
                    if(mydata.line_count <= AT_COMMAND_MAX_LINES && curr_col < AT_COMMAND_MAX_LINE_SIZE){
                        mydata.data[mydata.line_count-1][curr_col] = current_character;
                        curr_col++;
                    }
                    state = 15;
                }
                else{
                    printf("Received: %c\n", current_character);
                    printf("Expected: ASCII 32-128 or <CR>\n");
                    printf("Stopped in state: %d\n", state);
                    state = 0;
                    curr_col = 0;
                    return STATE_MACHINE_READY_WITH_ERROR;
                }
            }
            break;

        case 15:
            if(current_character == '\n'){
                if(mydata.line_count <= AT_COMMAND_MAX_LINES && curr_col < AT_COMMAND_MAX_LINE_SIZE){
                    mydata.data[mydata.line_count-1][curr_col] = current_character;
                    curr_col++;
                }
                state = 16;
            }
            else{
                printf("Received: %c\n", current_character);
                printf("Expected: <LF>\n");
                printf("Stopped in state: %d\n", state);
                state = 0;
                curr_col = 0;
                return STATE_MACHINE_READY_WITH_ERROR;
            }
            break;

        case 16:
            if(isException && current_character!='\r'){
                printf("Received: %c\n", current_character);
                printf("Expected: <CR>\n");
                printf("Stopped in state: %d\n", state);
                state = 0;
                curr_col = 0;
                return STATE_MACHINE_READY_WITH_ERROR;
            }

            switch(current_character){
                case '+':
                    curr_col = 0;
                    if(mydata.line_count <= AT_COMMAND_MAX_LINES && curr_col < AT_COMMAND_MAX_LINE_SIZE){
                        mydata.line_count++;
                        mydata.data[mydata.line_count-1][curr_col] = current_character;
                        curr_col++;
                    }
                    state = 14;
                    break;
                case '\r':
                    state = 17;
                    break;
                default:
                    printf("Received: %c\n", current_character);
                    printf("Expected: <CR> or +\n");
                    printf("Stopped in state: %d\n", state);
                    state = 0;
                    curr_col = 0;
                    return STATE_MACHINE_READY_WITH_ERROR;
            }
            break;

        case 17:
            if(current_character == '\n')
                state = 18;
            else{
                printf("Received: %c\n", current_character);
                printf("Expected: <LF>\n");
                printf("Stopped in state: %d\n", state);
                state = 0;
                curr_col = 0;
                return STATE_MACHINE_READY_WITH_ERROR;
            }
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
                    printf("Received: %c\n", current_character);
                    printf("Expected: O or E\n");
                    printf("Stopped in state: %d\n", state);
                    state = 0;
                    curr_col = 0;
                    return STATE_MACHINE_READY_WITH_ERROR;
            }
            break;
    }

    mydata.ok = 2;
    return STATE_MACHINE_NOT_READY;
}

void at_command_print(){
		int i = 0;
    for(i = 0; i < mydata.line_count; ++i)
        printf("%s", mydata.data[i]);

    if(mydata.ok == 1)
        printf("Status: OK\n");
    else
        printf("Status: ERROR\n");

    printf("\n");
}

void at_command_reset(){
    memset(mydata.data, 0, sizeof(mydata.data));
    mydata.ok = 2;
    mydata.line_count = 0;
}
