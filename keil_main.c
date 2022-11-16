#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "LPC177x_8x.h"
#include "system_LPC177x_8x.h"
#include <retarget.h>

#include <DRV\drv_sdram.h>
#include <DRV\drv_lcd.h>
#include <DRV\drv_uart.h>
#include <DRV\drv_touchscreen.h>
#include <DRV\drv_led.h>
#include <utils\timer_software.h>
#include <utils\timer_software_init.h>

#include "parser.h"

AT_COMMAND_DATA data_structure; 
const char* at_command_simple = "AT\r\n";
const char* at_command_csq = "AT+CSQ\r\n";
const char* at_command_creg = "AT+CREG?\r\n";
const char* at_command_cops= "AT+COPS?\r\n";
const char* at_command_gsn = "AT+GSN\r\n";
const char* at_command_gmi = "AT+GMI\r\n";
const char* at_command_gmr = "AT+GMR\r\n";
timer_software_handler_t my_timer_handler; 
timer_software_handler_t my_handler;
STATE_MACHINE_RETURN_VALUE last_returned;

void timer_callback_1(timer_software_handler_t h)
{
}

void testLCD()
{
	uint32_t i,j;
	LCD_PIXEL foreground = {0, 255, 0, 0};
	LCD_PIXEL background = {0, 0, 0, 0};
	
	
	for (i = 0; i < LCD_HEIGHT; i++)
	{
		for (j = 0; j < LCD_WIDTH / 3; j++)
		{
			DRV_LCD_PutPixel(i, j, 255, 0, 0);
		}
		for (j = LCD_WIDTH / 3; j < 2 * (LCD_WIDTH / 3); j++)
		{
			DRV_LCD_PutPixel(i, j, 230, 220, 0);
		}
		for (j = 2 * (LCD_WIDTH / 3); j < LCD_WIDTH; j++)
		{
			DRV_LCD_PutPixel(i, j, 0, 0, 255);
		}
	}

	DRV_LCD_Puts("Hello", 20, 30, foreground, background, TRUE);
	DRV_LCD_Puts("Hello", 20, 60, foreground, background, FALSE);	
}

void TouchScreenCallBack(TouchResult* touchData)
{
	printf("touched X=%3d Y=%3d\n", touchData->X, touchData->Y);		
	
}

void BoardInit()
{
	timer_software_handler_t handler;
	
	TIMER_SOFTWARE_init_system();
	
	
	DRV_SDRAM_Init();
	
	initRetargetDebugSystem();
	DRV_LCD_Init();
	DRV_LCD_ClrScr();
	DRV_LCD_PowerOn();	
	
	DRV_TOUCHSCREEN_Init();
	DRV_TOUCHSCREEN_SetTouchCallback(TouchScreenCallBack);
	DRV_LED_Init();
	printf ("Hello\n");	
	
	handler = TIMER_SOFTWARE_request_timer();
	TIMER_SOFTWARE_configure_timer(handler, MODE_1, 100, 1);
	TIMER_SOFTWARE_set_callback(handler, timer_callback_1);
	TIMER_SOFTWARE_start_timer(handler);
}
void MyTimerCallback(timer_software_handler_t handler)
{
 DRV_LED_Toggle(LED_1);
	printf("%c", 'c');
} 
void GetCommandResponse(uint8_t isException) 
{ 
 uint8_t ch; 
 BOOLEAN ready = FALSE; 
	
 TIMER_SOFTWARE_reset_timer(my_handler); 
 TIMER_SOFTWARE_start_timer(my_handler); 
 while ((!TIMER_SOFTWARE_interrupt_pending(my_handler)) && (ready == FALSE)) 
 { 
		
		while (DRV_UART_BytesAvailable(UART_3) > 0) 
		{ 
			DRV_UART_ReadByte(UART_3, &ch); 
			last_returned = at_command_parser(ch, isException);
			if (last_returned != STATE_MACHINE_NOT_READY) 
			{ 
				ready = TRUE; 
			} 
		} 
 } 
} 

void SendCommand(const char *command) 
{ 
 DRV_UART_FlushRX(UART_3); 
 DRV_UART_FlushTX(UART_3); 
 DRV_UART_Write(UART_3, (uint8_t*)command, strlen(command)); 
} 

void ExecuteCommand(const char *command, uint8_t isException) 
{ 
	SendCommand(command); 
	GetCommandResponse(isException); 
} 

uint32_t ConvertAsuToDbmw(uint32_t asu){
		return 2*asu - 113;
}

BOOLEAN CommandResponseValid(){
	 return last_returned == STATE_MACHINE_READY_OK;
}

uint32_t ExtractData(AT_COMMAND_DATA* mydata)
{
 uint32_t rssi = 0;
	uint32_t i;
	for(i = 6; i<128; ++i){
		if( mydata->data[0][i] != ','){
			rssi = rssi  * 10 + mydata->data[0][i] - '0';
		}
		else
			break;
	}
	
	return rssi;
}

void extract_data_creg(AT_COMMAND_DATA* mydata)
{
	uint8_t n = 0, state = 0, cell_id = 0, loc_id = 0;
	uint8_t i = strlen("+CREG: ");
	for(; i<128; ++i){
		if( mydata->data[0][i] != ','){
			n = n  * 10 + mydata->data[0][i] - '0';
		}
		else
			break;
	}
	
	for(++i; i<128; ++i){
		if( mydata->data[0][i] != ',' &&  mydata->data[0][i] != '\r'){
			state = state  * 10 + mydata->data[0][i] - '0';
		}
		else
			break;
	}
	
	printf("State of registration: %d\n", state);
	
	if(n == 2 && state != 3){
			for(++i; i<128; ++i){
				if( mydata->data[0][i] != ',' ){
					cell_id = cell_id  * 10 + mydata->data[0][i] - '0';
				}
				else
					break;
			}
			
			for(++i; i<128; ++i){
				if( mydata->data[0][i] != ','){
					loc_id = loc_id  * 10 + mydata->data[0][i] - '0';
				}
				else
					break;
			}
	}
}

int main(void)
{
//	uint8_t ch;
		uint32_t rssi_value_asu; 
	uint32_t rssi_value_dbmw; 
//	timer_software_handler_t handler; // declare a software timer
	BoardInit();
	testLCD();
	TIMER_SOFTWARE_init();
	TIMER_SOFTWARE_init_system();

	DRV_UART_Configure(UART_3, UART_CHARACTER_LENGTH_8, 115200, UART_PARITY_NO_PARITY, 1, TRUE, 3);
	DRV_UART_Configure(UART_0, UART_CHARACTER_LENGTH_8, 115200, UART_PARITY_NO_PARITY, 1, TRUE, 3);
		
		my_timer_handler = TIMER_SOFTWARE_request_timer();
	 TIMER_SOFTWARE_configure_timer(my_timer_handler, MODE_1, 1000, true); 
	
	DRV_UART_Write(UART_3, (uint8_t *)at_command_simple, strlen(at_command_simple)); 
	TIMER_SOFTWARE_Wait(1000); 
	DRV_UART_Write(UART_3, (uint8_t *)at_command_simple, strlen(at_command_simple)); 
	TIMER_SOFTWARE_Wait(1000); 
	DRV_UART_Write(UART_3, (uint8_t *)at_command_simple, strlen(at_command_simple)); 
	TIMER_SOFTWARE_Wait(1000); 
	printf("Done");
	

	TIMER_SOFTWARE_start_timer(my_timer_handler);
	while(1) 
	{ 
		 if (TIMER_SOFTWARE_interrupt_pending(my_timer_handler)) 
		 { 
			   ExecuteCommand(at_command_csq, 0); 
				 if (CommandResponseValid()) 
				 { 
						rssi_value_asu = ExtractData(&mydata); 
						rssi_value_dbmw = ConvertAsuToDbmw(rssi_value_asu); 
						printf("GSM Modem signal %d ASU -> %d dBmW\n", rssi_value_asu, rssi_value_dbmw); 
				 } 
				 
				 
				 ExecuteCommand(at_command_creg, 0); 
				 if (CommandResponseValid()) 
				 { 
					 at_command_print();
					 extract_data_creg(&mydata);
				 }
				 
				 
				 ExecuteCommand(at_command_cops, 0); 
				 if (CommandResponseValid()) 
				 { 
					 at_command_print();
					 printf("cops - ok\n");
				 }
				 
				 
				 ExecuteCommand(at_command_gsn, 1); 
				 if (CommandResponseValid()) 
				 { 
					 at_command_print();
					 printf("gsn - ok\n");
				 }
				 
				 
				 ExecuteCommand(at_command_gmi, 1); 
				 if (CommandResponseValid()) 
				 { 
					 at_command_print();
					 printf("gmi - ok\n");
				 }
				 
				 ExecuteCommand(at_command_gmr, 1); 
				 if (CommandResponseValid()) 
				 { 
					 at_command_print();
					 printf("gmr - ok\n");
				 }
				 
				 
				 TIMER_SOFTWARE_clear_interrupt(my_timer_handler); 
		 } 
	} 
	
	
	return 0; 
}
