#include "..\..\..\drivers\TCPIPcore.h"
#include "loopback.h"
#include "lcd.h"

#define	MAX_BUF_SIZE	8192		// Maximum receive buffer size

/*******************************************************************************************/
/* If you have QFN64pin package W7100A, have to use 'USE_QFN64' definition in TCPIPcore.h  */
/*******************************************************************************************/
						
uint8 xdata * data_buf =(uint8 xdata *)0x7000;	// Position of receive buffer

void main()
{
	uint8 xdata str[15];

	Init_iMCU();		// Initialize   iMCUW7100A  
	Init_Network(); 	// Initialize   Network Configuration

	// for LCD	
	lcd_init(); 		// Initialize   TextLCD 
	sprintf(str, "%.3bu.%.3bu.%.3bu.%.3bu",
			 IINCHIP_READ (SIPR0+0),  IINCHIP_READ (SIPR0+1), 
			 IINCHIP_READ (SIPR0+2),  IINCHIP_READ (SIPR0+3));
	evb_set_lcd_text(0,(uint8 *) "iMCU7100EVB");
	evb_set_lcd_text(1, str);

		//start message
	printf("\r\n-------LOOPBACK_UDP_START--------\r\n");
	while(1){
			loopback_udp(0, 3000,data_buf, 0);	
			loopback_udp(1, 3001,data_buf, 0);	
			loopback_udp(2, 3002,data_buf, 0);	
			loopback_udp(3, 3003,data_buf, 0);	
			loopback_udp(4, 3004,data_buf, 0);	
			loopback_udp(5, 3005,data_buf, 0);	
			loopback_udp(6, 3006,data_buf, 0);	
			loopback_udp(7, 3007,data_buf, 0);	
	}

}



