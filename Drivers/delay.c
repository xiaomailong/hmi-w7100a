/**********************************************
(c)COPYRIGHT WIZnet Inc. ALL RIGHT RESERVED
	For W7100/W7100A/W7100A-QFN64 chip
**********************************************/
/*
*
@file		delay.c
@brief	delay functions for the W7100
*
*/

#include "delay.h"

/*****************************************************************************************
	Function name: wait_1us
	Input		:	cnt; Delay duration = cnt * 1u seconds
	Output	:	non
	Description
	: A delay function for waiting cnt*1u second.
*****************************************************************************************/
void wait_1us(unsigned int cnt)
{
	unsigned int i;
	
	for(i = 0; i<cnt; i++) {
#pragma ASM
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
		NOP
#pragma ENDASM
		}
}


/*****************************************************************************************
	Function name: wait_1ms
	Input		:	cnt; Delay duration = cnt * 1m seconds
	Output	:	non
	Description
	: A delay function for waiting cnt*1m second. This function use wait_1us but the wait_1us
		has some error (not accurate). So if you want exact time delay, please use the Timer.
*****************************************************************************************/
void wait_1ms(unsigned int cnt)
{
	unsigned int i;
	for (i = 0; i < cnt; i++) wait_1us(1000);
}

/*****************************************************************************************
	Function name: wait_10ms
	Input		:	cnt; Delay duration = cnt * 10m seconds
	Output	:	non
	Description
	: A delay function for waiting cnt*10m second. This function use wait_1ms but the wait_1ms
		has some error (not accurate more than wait_1us). So if you want exact time delay, 
		please use the Timer.
*****************************************************************************************/
void wait_10ms(unsigned int cnt)
{
	unsigned int i;
	for (i = 0; i < cnt; i++) wait_1ms(10);
}

