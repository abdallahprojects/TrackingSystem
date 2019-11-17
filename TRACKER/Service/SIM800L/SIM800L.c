/*
 * SIM800L.c
 *
 * Created: 11/2/2019 12:05:26 PM
 *  Author: asere
 */ 
//external includes
#include <avr/io.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <HWuart.h>
#include <SWuart.h> // for debugging info
#include <LEDs.h>
// internal include
#include "SIM800L.h"
#include "SIM800L_Loc.h"

void SIM800L_Init(void){
	HWuart_Init(19200);
	sim800l.tx = printf;
	sim800l.rxWaitFor = rxWaitFor;
	sim800l.readIpAddress = readIpAddress;
	LEDS_Init();
}

void SIM800L_Cyclic5ms(void)
{
	// Enable HW printf
	ENABLE_HW_PRINTF();
	switch(SIM800_Status)
	{
		case SIM800_TestAliv:
			TestOK();
			break;
		case SIM800_SetErrorMode:
			SetErrorMode();
			break;
		case SIM800_NetRegister:
			NetRegister();
			break;
		case SIM800_NetRegisterOk:
			NetRegisterOk();
			break;
		case SIM800_NetAtt: // Attach to the network
			NetAtt();
			break;
		case SIM800_SetAccessPoint:
			SetAccessPoint();
			break;
		case SIM800_SetGprsConn:
			SetGprsConn();
			break;
		case SIM800_ReadIpAdd:
			ReadIpAdd();
			break;
		case SIM800_StartUDP:
			StartUDP();
			break;
		case SIM800_waitForData:
			waitForData();
			break;
		case SIM800_SendCommand:
			SendCommand();
			break;
		case SIM800_SendData:
			SendData();
			break;
		case SIM800_CloseConn:
			CloseConn();
			break;
		case SIM800_endFun:
			endFun();
			break;
		case SIM800_RxWaiting:
			waiting();
			break;
		case SIM800_IpWaiting:
			IpWaiting();
			break;
		default:
			 __asm__ __volatile__ ("nop" ::);
			//Error();
	}
}


void TestOK(void){
	sim800l.stat = sim800_init;
	sim800l.tx("AT\r\n");
	sim800l.rxWaitFor(COMMAND("OK"),2000,SIM800_SetErrorMode); // wait for 5 seconds if not return back
}

void SetErrorMode(void){
	sim800l.tx("ATE1\r\n");
	sim800l.rxWaitFor(COMMAND("OK"),2000,SIM800_NetRegister); // wait for 5 seconds if not return back
}

void NetRegister(void) {
	sim800l.tx("AT+CREG?\r\n");
	sim800l.rxWaitFor(COMMAND("+CREG: 0,1"),2000,SIM800_NetRegisterOk); // wait for 5 seconds if not return back
}
void NetRegisterOk(void) {
	sim800l.rxWaitFor(COMMAND("OK"),2000,SIM800_NetAtt); // wait for 5 seconds if not return back
}
void NetAtt(void) {
	LED_ON(LED2);
	sim800l.stat = sim800_network_connected;
	sim800l.tx("at+CGACT=1,1\r\n");
	sim800l.rxWaitFor(COMMAND("OK"),2000,SIM800_SetAccessPoint); // wait for 5 seconds if not return back
	ENABLE_SW_PRINTF();
	printf("Attaching to the network !\r\n");
	ENABLE_HW_PRINTF();
	//SIM800_Status = SIM800_END;

}
void SetAccessPoint(void) {
	LED_ON(LED2);
	sim800l.stat = sim800_connecting_to_internet;
	sim800l.tx("AT+CSTT=\"fast.t-mobile.com\"\n");
	sim800l.rxWaitFor(COMMAND("OK"),1000,SIM800_SetGprsConn); // wait for 5 seconds if not return back
	ENABLE_SW_PRINTF();
	printf("Registering access Point !\r\n");
	ENABLE_HW_PRINTF();
	//SIM800_Status = SIM800_END;

}
void SetGprsConn(void) {
	LED_ON(LED2);
	sim800l.tx("AT+CIICR\r\n");
	sim800l.rxWaitFor(COMMAND("OK"),1000,SIM800_ReadIpAdd); // wait for 5 seconds if not return back
	ENABLE_SW_PRINTF();
	printf("Bring up GPRS !\r\n");
	ENABLE_HW_PRINTF();
	//SIM800_Status = SIM800_END;

}
void ReadIpAdd(void){
	sim800l.tx("AT+CIFSR\n");
	sim800l.readIpAddress(1000,SIM800_StartUDP);
}
void StartUDP(void) {
	LED_ON(LED2);
	sim800l.tx("AT+CIPSTART=\"UDP\",\"68.54.10.8\",\"5051\"\n");
	sim800l.rxWaitFor(COMMAND("OK"),1000,SIM800_waitForData); // wait for 5 seconds if not return back
	ENABLE_SW_PRINTF();
	printf("Setting up UDP !\r\n");
	ENABLE_HW_PRINTF();
	//SIM800_Status = SIM800_END;

}
void waitForData(void){
	sim800l.stat = sim800_ready;
	// waiting to send data
}
void SendCommand(void) {
	LED_ON(LED2);
	sim800l.tx("AT+CIPSEND=%d\r\n",UDP.txtSize);
	sim800l.rxWaitFor(COMMAND(">"),1000,SIM800_SendData); // wait for 5 seconds if not return back
	ENABLE_SW_PRINTF();
	printf("Sending UDP packet size !\r\n");
	ENABLE_HW_PRINTF();
	//SIM800_Status = SIM800_END;
}
void SendData(void) {
	LED_ON(LED2);
	sim800l.tx("%s",UDP.txt);
	sim800l.rxWaitFor(COMMAND("OK"),1000,SIM800_waitForData); // wait for 5 seconds if not return back
	ENABLE_SW_PRINTF();
	printf("Sending UDP data !\r\n");
	ENABLE_HW_PRINTF();
	//SIM800_Status = SIM800_END;
}
void CloseConn(void) {
	LED_ON(LED2);
	sim800l.tx("AT+CIPCLOSE\r\n");
	sim800l.rxWaitFor(COMMAND("OK"),1000,SIM800_endFun); // wait for 5 seconds if not return back
	ENABLE_SW_PRINTF();
	printf("Closing Connection !\r\n");
	ENABLE_HW_PRINTF();
	//SIM800_Status = SIM800_END;
}
void endFun(void) {
	LED_ON(LED3);
}
void waiting(void){

		static uint8_t i = 0;
		uint8_t in_ch;
		wait.currentTime++;
		if(wait.currentTime < wait.waitTime)
		{
			while(HWuart_Rx(&in_ch) & (HWuart_Rx_OK|HWuart_Rx_OK_Again))
				{
					if(in_ch == wait.compare_string[i])
					{
						i++;
						}else if(in_ch == wait.compare_string[0]){
							i = 1;
						}else{
							i = 0;
					}
					if(i == wait.string_size -1)
					{
						SIM800_Status = wait.next_state;
					}
				}
		}else{
			SIM800_Status = wait.prev_state;
			i = 0;
		}
}
void IpWaiting(void){

		static uint8_t i = 0;
		static uint8_t ip_indx = 0;
		uint8_t indx;
		uint8_t in_ch;
		wait.currentTime++;
		if(wait.currentTime < wait.waitTime)
		{
			while(HWuart_Rx(&in_ch) & (HWuart_Rx_OK|HWuart_Rx_OK_Again))
				{
					if(in_ch == '.')
					{
						i++;
						ip_indx = 0;
					}else if(in_ch == '\n'){
						/* Can't have a new line while reading the IP
						 * Address, reset everything, otherwise finish  */
						if(i == 3)
						{
							i = 0;
							ip_indx = 0;
							SIM800_Status = wait.next_state;
						}else{
							ip_indx = 0;
							i = 0;
							memcpy(IpAddress,emptyIpAdd,sizeof(IpAddress));
						}
					}else{
						if((in_ch >= 48)&&(in_ch <= 57))
						{ // The input character is between 0 and 9 decimal
							if(ip_indx < 3)
							{
								for(indx=ip_indx;indx>0;indx--) // queue the ip numbers
									IpAddress[((i*4)+2)-(indx)] = IpAddress[((i*4)+2)-(indx-1)];
								ip_indx++;
								IpAddress[((i*4)+2)] = in_ch;
							}else{
								/* since numbers of digits exceeded 3, it cannot be
								 * IP address, therefore reset everything */
								ip_indx = 0;
								i = 0;
								memcpy(IpAddress,emptyIpAdd,sizeof(IpAddress));
							}

						}
					}
				}
		}else{
			SIM800_Status = wait.prev_state;
			i = 0;
		}
}
/* this interface in uart.waitFor, to wait for input until next state is achieved, otherwise
 * the state machine will jump back to the previous state and try again
 */
void rxWaitFor(char * ch,uint8_t size,uint16_t timeout,SIM800_Status_t next_state){
	wait.waitTime = timeout;
	wait.currentTime = 0;
	wait.compare_string = ch;
	wait.next_state = next_state;
	wait.string_size = size;
	wait.prev_state = (SIM800_Status == SIM800_TestAliv)? SIM800_Status : (SIM800_Status);
	SIM800_Status = SIM800_RxWaiting;
}
void readIpAddress(uint16_t timeout,SIM800_Status_t next_state)
{
	wait.waitTime = timeout;
	wait.currentTime = 0;
	wait.compare_string = "...";
	wait.next_state = next_state;
	wait.string_size = 3;
	wait.prev_state = SIM800_Status;
	SIM800_Status = SIM800_IpWaiting;
}
// will send UDP frame with size max 255 bytes
state_t SIM800L_SendUDP(char *ch,uint8_t size){
	state_t ret = SIM800L_send_ok;
	uint8_t i;
	if(sim800l.stat == sim800_ready)
	{
		if(ch != NULL){
			ch[size] = 0;
			for(i=0;i<size;i++){
				if(ch[i]==0){
					ret = SIM800L_wrongSize;
				}
			}
			if(ret == SIM800L_send_ok)
			{
				UDP.txt = ch;
				UDP.txtSize = size;
				SIM800_Status = SIM800_SendCommand;
				sim800l.stat = sim800_sending_data;
			}
		}else{
			ret = SIM800L_null_pointer;
		}

	}else{
		ret = SIM800L_not_ready;
	}
	return ret;

}
