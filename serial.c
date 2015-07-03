#include "W7100.h"
#include "serial.h"

void InitSerial(void)
{
	ET1 = 0;		/* TIMER1 INT DISABLE */
	TMOD = 0x20;  	/* TIMER MODE 2 */
	PCON |= 0x80;		/* SMOD = 1   ²¨ÌØÂÊXTALµÄ1/2 */	  


	TH1 = 0xFC;		/* X2 115200(SMOD=1) at 88.4736 MHz */
	TR1 = 1;		/* START THE TIMER1 */	
	SCON = 0x52;		/* SERIAL MODE 1, REN=1, TI=1, RI=0 */

	/* Interrupt */
	ES0   = 0;	 	/* Serial interrupt disable */
	RI   = 0; 		
	TI   = 0; 		
}