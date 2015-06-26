/**********************************************
(c)COPYRIGHT WIZnet Inc. ALL RIGHT RESERVED
 W7100 driver
**********************************************/
/*
*
@file		wizmemcpy.c
@brief	Define memory copy function between MCU memory and TCPIPCore memory
*
*/
			   
#include "TCPIPCore.h"

//#define GENERAL_APP
//#define FAST_APP

void FCOPY_IN_C(void);
							 
void wizmemcpy(unsigned long fsrc, unsigned long fdst, unsigned int len)  small
{
/*************************************************************************/
// General version
/* Below wizmemcpy function is for general application 
   It has relatively slow copying speed but very stable	
   WIZnet recommand this stable code */


#if defined GENERAL_APP
	register temp;
	unsigned char EAback = EA;
	unsigned char tmp_DPX0, src_DPX0, dst_DPX0;
	tmp_DPX0 = DPX0;
	src_DPX0 = (unsigned char)(fsrc>>16);
	dst_DPX0 = (unsigned char)(fdst>>16);
		

	EA = 0;
	while(len--)
	{
		DPX0 = src_DPX0;
		temp = *((volatile unsigned char xdata*)((unsigned int)fsrc++));
		DPX0 = dst_DPX0;
		*((volatile unsigned char xdata*)(unsigned int)fdst++) = temp;
	}
	EA = EAback;
	DPX0 = tmp_DPX0;
   
#elif defined FAST_APP
/*************************************************************************/

// Fast version
/* Below wizmemcpy function specialized for speed
   It has relatively unstable but very fast copying speed.
   This function may only valid in Keil compiler 
   WIZnet not recommand fast version because of its stability */
 
	unsigned char  eaback = EA;
	DPX0BK = DPH0;
	DPX1BK = DPL0;

	DPX0 = (unsigned char)(fsrc>>16);
	DPH0 = (unsigned char)(fsrc>>8);
	DPL0 = (unsigned char)(fsrc);

	DPX1 = (unsigned char)(fdst>>16);
	DPH1 = (unsigned char)(fdst>>8);
	DPL1 = (unsigned char)(fdst);

	RAMEA16 = len;	 	 

	EA = 0;
	WCONF &= ~(0x40);     // Enable ISP Entry
	((void(code*)(void))ENTRY_FCOPY)();
	DPH0 = DPX0BK;
	DPL0 = DPX1BK;

	DPX0 = 00;
	DPX1 = 00;
	WCONF |= 0x40;        // Disable ISP Entry

	EA = eaback;

#else

	unsigned char  eaback = EA;
	DPX0BK = DPH0;
	DPX1BK = DPL0;

	DPX0 = (unsigned char)(fsrc>>16);
	DPH0 = (unsigned char)(fsrc>>8);
	DPL0 = (unsigned char)(fsrc);

	DPX1 = (unsigned char)(fdst>>16);
	DPH1 = (unsigned char)(fdst>>8);
	DPL1 = (unsigned char)(fdst);

	RAMEA16 = len;	 	 

	EA = 0;
	//WCONF &= ~(0x40);     // Enable ISP Entry
	//((void(code*)(void))ENTRY_FCOPY)();
	FCOPY_IN_C();
	DPH0 = DPX0BK;
	DPL0 = DPX1BK;

	DPX0 = 00;
	DPX1 = 00;
	WCONF |= 0x40;        // Disable ISP Entry

	EA = eaback;
 
#endif
}


#ifndef GENERAL_APP
#ifndef FAST_APP 

void FCOPY_IN_C(void)
{
	#pragma ASM
	    MOV     DPSBK, DPS
		MOV 	DPS, #20h;	    
		MOV  	A, RAMEA16;
	    JNZ		L0002;
	    MOV     A, 0FDh;	  // 0FDh => RAMEA16 + 01h
		JZ      L0003;
	
	L0001:
	    DEC     0FDh;
	
	L0002:
	    MOVX    A, @DPTR;
		MOVX    @DPTR, A;
		INC     DPTR;
		INC     DPTR;
		DEC     RAMEA16;
		MOV     A, RAMEA16;
		JNZ     L0002;
	
		MOV     A, 0FDh;
		JNZ     L0001;
	
	L0003:
	    MOV     DPS, DPSBK;
	    RET;
	#pragma ENDASM
}
#endif
#endif

