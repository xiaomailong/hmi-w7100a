#include <stdio.h>
#include "lcd.h"
#include "serial.h" 		// serial related functions
#include "socket.h" 		// W7100A driver file
#include "types.h"
#include "w7100.h"
#include "Wizmemcpy.h"
#include "TCPIPcore.h"
#include "delay.h"
#include "loopback.h"

#define	MAX_BUF_SIZE	8192		// Maximum receive buffer size
void Init_iMCU(); 		
void Init_Network();	
uint8 xdata * data_buf =(uint8 xdata *)0x7000;	// Position of receive buffer
void wd_init(void);
void wd_resetTimer(void);
void chip_reset(void);


void chip_reset(void)
{
#pragma ASM
		POP  ACC  //; pop return address
		POP  ACC
		CLR  A    //; push 0 as new
		PUSH ACC  //; return address to stack
		PUSH ACC
		RETI      //; execute return of interrupt
#pragma ENDASM
}

void wd_init(void)
{
	uint8 tmpEA;

	tmpEA = EA;
	EA = 0;

	CKCON |= 0xc0;	  					// CKCON.7-6 = WD[1:0] : Watchdog Interval
	TA=0xAA;TA=0x55;WDCON = 0x00;		// WDCON Clear

	TA=0xAA;TA=0x55;EWT = 0;			// Disable Watchdog Timer Reset (WDCON)	
	TA=0xAA;TA=0x55;WDIF = 0;			// Watchdog Interrupt flag clear (WDCON)	
	EWDI = 1;		   					// Enable Watchdog Timer Interrupt (EIE)		

	EA = tmpEA;	   					
}

void wd_resetTimer(void)
{
 	TA=0xAA;TA=0x55;RWT=1;
}

void wd_isr(void) interrupt 12
{
 	static uint8 int_cnt = 0;
	uint8 tmpEA;

	tmpEA = EA;					// Backup global interrupt status
 	EA = 0;	 					// Global interrupt disable
	
	TA=0xAA; TA=0x55; WDIF = 0; // Watchdog interrupt flag clear
	P3_7 = ~P3_7;				//FPGA外部看门狗
	int_cnt++;
	if (int_cnt >= 2)
	{
		int_cnt = 0;
		chip_reset(); 				// Chip reset
	}

	EA = tmpEA;				   	// Global interrupt restoration
}
void main()
{
	Init_iMCU();		// Initialize   iMCUW7100     
	Init_Network(); 	// Initialize   Network Configuration
	wd_init();
	EA = 1;				/* Enable global interrupt */
	printf("\r\n-------LOOPBACK_UDP_START--------\r\n");
		  	
	WCONF &=~(0x10); //EM[2:0]:101
	WCONF|=0x68;

	while(1){
			loopback_udp(0, 3001,data_buf, 0);
			P3_6 = ~P3_6;			//FPGA外部看门狗
			wd_resetTimer();		//喂狗
	}

}


/*
********************************************************************************
*              8051 Initialization Function
*
* Description: 
* Arguments  : None.
* Returns    : None.
* Note       : 
********************************************************************************
*/
void Init_iMCU(void)
{ 
	uint8 xdata i;

	EA = 0; 		// Disable all interrupts 	 /*中断总开关*/
	CKCON = 0x02;		// External Memory Access Time 脉冲宽度3
	WTST = 0x03;
	EXTWTST0=0x00;	  //transmit time(external memory takes)
	EXTWTST1=0x00;

	P0_PU=0xff;		/* All P0 ports ull up */
	P0=0xff;

	P1_PU=0xff;		/* All P1 ports ull up */
	P1=0xff;

	P2_PU=0xff;		/* All P2 ports ull up */
	P2=0xff;

	P3_PU=0xff;		/* All P3 ports ull up */
	P3=0xff;

#ifdef __DEF_IINCHIP_INT__
	PINT5 = 1;	
	IINCHIP_WRITE(IMR,0xff);
	for(i=0;i<MAX_SOCK_NUM;i++) IINCHIP_WRITE(Sn_IMR(i),0x1f);	
//	IINCHIP_WRITE (INTLEVEL0+0,0x2b);
//	IINCHIP_WRITE (INTLEVEL0+1,0x01);
	EINT5 = 1;
#endif
	
	InitSerial(); 	// Initialize serial port (Refer to serial.c)
}

void Init_Network(void)
{
	uint8 xdata ip[4] = {192,168,2,3};                   // for setting SIP register
	uint8 xdata gw[4] = {192,168,2,1};                     // for setting GAR register
	uint8 xdata sn[4] = {255,255,255,0};                     // for setting SUBR register
	uint8 xdata mac[6] = {0x00,0x08,0xDC,0x00,0x02,0x03};      // for setting SHAR register
	uint8 xdata str[17];
	
	/* Assign the TX, RX memory of each SOCKETs */	 
	uint8 xdata txsize[MAX_SOCK_NUM] = {8,2,1,1,1,1,1,1};
	uint8 xdata rxsize[MAX_SOCK_NUM] = {8,2,1,1,1,1,1,1};

	/* Write MAC address of W7100 to SHAR register */
	IINCHIP_WRITE (SIPR0+0,ip[0]);
	IINCHIP_WRITE (SIPR0+1,ip[1]);
	IINCHIP_WRITE (SIPR0+2,ip[2]);
	IINCHIP_WRITE (SIPR0+3,ip[3]);
	
	/* Write GATEWAY of W7100 to GAR register */
	IINCHIP_WRITE (GAR0+0, gw[0]);
	IINCHIP_WRITE (GAR0+1, gw[1]);
	IINCHIP_WRITE (GAR0+2, gw[2]);
	IINCHIP_WRITE (GAR0+3, gw[3]);
	/* Write SUBNETMASK of W7100 to SURR register */
	IINCHIP_WRITE (SUBR0+0, sn[0]);
	IINCHIP_WRITE (SUBR0+1, sn[1]);
	IINCHIP_WRITE (SUBR0+2, sn[2]);
	IINCHIP_WRITE (SUBR0+3, sn[3]);
	/* Write MAC address of W7100 to SHAR register */
	IINCHIP_WRITE (SHAR0+0, mac[0]);
	IINCHIP_WRITE (SHAR0+1, mac[1]);
	IINCHIP_WRITE (SHAR0+2, mac[2]);
	IINCHIP_WRITE (SHAR0+3, mac[3]);
	IINCHIP_WRITE (SHAR0+4, mac[4]);
	IINCHIP_WRITE (SHAR0+5, mac[5]);

    set_MEMsize(txsize,rxsize);

  evb_set_lcd_text(1,str);	
}