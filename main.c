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



void main()
{
	Init_iMCU();		// Initialize   iMCUW7100
//	lcd_init();  
	//display CH_LCD
//	evb_set_lcd_text(0,(uint8 *) "iMCU7100EVB");     
	Init_Network(); 	// Initialize   Network Configuration
//	data_buf="test UDP";
		//start message
	printf("\r\n-------LOOPBACK_UDP_START--------\r\n");

	//	EA = 1;	
//	WCONF|=0x28;		  		/* Enable global interrupt */
	WCONF &=~(0x10); //EM[2:0]:101
	WCONF|=0x68;

	while(1){
			loopback_udp(0, 3001,data_buf, 0);
			printf("\r\n-------4--------\r\n");
			P3_7 = ~P3_7;
			printf("\r\n-------5--------\r\n");	
		//	loopback_udp(1, 3001,data_buf, 0);	
		//	loopback_udp(2, 3002,data_buf, 0);	
		//	loopback_udp(3, 3003,data_buf, 0);	
		//	loopback_udp(4, 3004,data_buf, 0);	
		//	loopback_udp(5, 3005,data_buf, 0);	
		//	loopback_udp(6, 3006,data_buf, 0);	
		//	loopback_udp(7, 3007,data_buf, 0);	
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
	IINCHIP_WRITE(IMR,0xff);
	for(i=0;i<MAX_SOCK_NUM;i++) IINCHIP_WRITE(Sn_IMR(i),0x1f);	
	IINCHIP_ISR_ENABLE();
#endif	
	InitSerial(); 	// Initialize serial port (Refer to serial.c)
	EA = 1;
}

void Init_Network(void)
{

	uint8 xdata ip[4] = {192,168,2,4};                   // for setting SIP register
	uint8 xdata gw[4] = {192,168,2,1};                     // for setting GAR register
	uint8 xdata sn[4] = {255,255,255,0};                     // for setting SUBR register
	uint8 xdata mac[6] = {0x00,0x08,0xDC,0x00,0x02,0x04};      // for setting SHAR register
	uint8 xdata str[17];
	
	/* Assign the TX, RX memory of each SOCKETs */	 
	uint8 xdata txsize[MAX_SOCK_NUM] = {8,2,1,1,1,1,1,1};
	uint8 xdata rxsize[MAX_SOCK_NUM] = {8,2,1,1,1,1,1,1};
//	 uint8 xdata txsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};
//	 uint8 xdata rxsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};

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

//	while(!(EIF & 0x02));
//	EIF &= ~0x02;

//	printf( "==============================================\r\n");
//	printf( "       W7100   Net Config Information         \r\n");
//	printf( "==============================================\r\n");

//	printf( "MAC ADDRESS IP : %.2x.%.2x.%.2x.%.2x.%.2x.%.2x\r\n", 
//		(int)IINCHIP_READ (SHAR0+0), (int)IINCHIP_READ (SHAR0+1), 
//		(int)IINCHIP_READ (SHAR0+2), (int)IINCHIP_READ (SHAR0+3),
//		(int)IINCHIP_READ (SHAR0+4), (int)IINCHIP_READ (SHAR0+5)
//		) ;
// 	sprintf(str,"%.3d.%.3d.%.3d.%.3d",
//			(int)IINCHIP_READ (SUBR0+0), (int)IINCHIP_READ (SUBR0+1), 
//			(int)IINCHIP_READ (SUBR0+2), (int)IINCHIP_READ (SUBR0+3));
//	printf( "SUBNET MASK  : %s\r\n",str); 

// 	sprintf(str,"%.3d.%.3d.%.3d.%.3d",
//			(int)IINCHIP_READ (GAR0+0), (int)IINCHIP_READ (GAR0+1), 
//			(int)IINCHIP_READ (GAR0+2), (int)IINCHIP_READ (GAR0+3));
//
//	printf( "G/W IP ADDRESS : %s\r\n",str);

// 	sprintf(str,"%.3d.%.3d.%.3d.%.3d",
//			(int)IINCHIP_READ (SIPR0+0), (int)IINCHIP_READ (SIPR0+1), 
//			(int)IINCHIP_READ (SIPR0+2), (int)IINCHIP_READ (SIPR0+3));

//	printf( "LOCAL IP ADDRESS : %s\r\n", str);

  //display CH_LCD
  evb_set_lcd_text(1,str);	

 

}

