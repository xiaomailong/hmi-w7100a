#include "W7100.h"
#include "serial.h"

/*
Description   :  Initialization of Serial Port(Ex.Baud Rate setting)
Argument      :  
Return Value  :  
Note          :  
*/
void InitSerial(void)
{
#ifdef APP_UART				   
#if defined UART_MODE1_TIMER1
	SCON = 0x50;	// SERIAL MODE 1, SM00 = 0, SM01 =1, REN=1
	TMOD |= 0x20;	// Timer1 MODE 2
	PCON |= 0x80;	// SMOD0 = 1
	TL1 = 0xFC;		// Baud Rate Setting to 115200bps
	TH1 = 0xFC;		// Baud Rate Setting to 115200bps
	TR1 = 1;		// Timer1 START
#elif defined UART_MODE1_TIMER2 
	SCON = 0x50;	// SERIAL MODE 1, SM00 = 0, SM01 =1, REN=1
	T2CON = 0x30;	// Timer2 Baud Rate Generator MODE
	TH2 = 0xFF;		// Baud Rate Setting to 115200bps
	TL2 = 0xE8;
	RLDH = 0xFF;	// Reload Baud Rate Setting to 115200bps
	RLDL = 0xE8;
	TR2 = 1;		// Timer2 START
#elif defined UART_MODE3_TIMER1 
	SCON = 0xD0;	// SERIAL MODE 3, SM00 = 1, SM01 =1, REN=1
	TMOD |= 0x20;	// Timer1 MODE 2
	PCON |= 0x80;	// SMOD0 = 1
	TL1 = 0xFC;		// Baud Rate Setting to 115200bps
	TH1 = 0xFC;		// Baud Rate Setting to 115200bps
	TR1 = 1;		// Timer1 START
#elif defined UART_MODE3_TIMER2 
	SCON = 0xD0;	// SERIAL MODE 3, SM00 = 1, SM01 =1, REN=1
	T2CON = 0x30;	// Timer2 Baud Rate Generator MODE
	TH2 = 0xFF;		// Baud Rate Setting to 115200bps
	TL2 = 0xE8;		// Baud Rate Setting to 115200bps
	RLDH = 0xFF;	// Reload Baud Rate Setting to 115200bps
	RLDL = 0xE8;	// Reload Baud Rate Setting to 115200bps
	TR2 = 1;		// Timer2 START	
#endif
#endif

#ifndef APP_UART
	ET1 = 0;		/* TIMER1 INT DISABLE */
	TMOD = 0x20;  	/* TIMER MODE 2 */
	PCON |= 0x80;		/* SMOD = 1 */						  
	TH1 = 0xFC;		/* X2 115200(SMOD=1) at 88.4736 MHz */
	TR1 = 1;		/* START THE TIMER1 */	
	SCON = 0x52;		/* SERIAL MODE 1, REN=1, TI=1, RI=0 */
	/* Interrupt */
	ES0 = 0;	 	/* Serial interrupt disable */
	RI = 0; 		
	TI = 0; 				
#endif		
}
/*
Description   :  Initialization of Serial Port - Serial interrupt enable
Argument      :  
Return Value  :  
Note          :  
*/
void InitSerial_Interrupt(void)
{
	/* Interrupt */	
	RI = 0; 		
	TI = 0; 			
	ES0	= 1; 		/* Serial interrupt enable, if you use serial interrupt, you must  */
}

void putbyte (char byData)
{
	// Write data into serial-buffer.
	SBUF = byData; 
	// Wait till data recording is finished.
	while(!TI);
	TI = 0;
}