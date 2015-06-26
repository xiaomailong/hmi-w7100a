#include "../Drivers/TCPIPcore.h"
#include "../Drivers/serial.h"
#include "loopback.h"
#include "lcd.h"

/*******************************************************************************************/
/* If you have QFN64pin package W7100A, have to use 'USE_QFN64' definition in TCPIPcore.h  */
/*******************************************************************************************/

uint8 xdata * data_buf = (uint8 xdata *)0x7000;    // Position of receive buffer

const uint16 port = 3001;

void main()
{
    Init_iMCU();        // Initialize iMCUW7100A  
    InitSerial();       // Initialize serial
    Init_Network();     // Initialize Network Configuration

    printf("LOOPBACK_UDP_START\n");
    while (1){
        loopback_udp(0, port, data_buf, 0);
    }

}



