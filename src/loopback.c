#include "..\..\..\drivers\TCPIPcore.h"	 
#include "loopback.h"

//#define DEBUG_PRINT

void loopback_udp(SOCKET s, uint16 port, uint8 xdata * data_buf, uint16 mode)
{
  uint16 xdata len;
  uint8 xdata destip[4];
  uint16 xdata destport;

  switch(IINCHIP_READ(Sn_SR(s)))
  {
    case SOCK_UDP:
    if((len=getSn_RX_RSR(s)) > 0)                   // check the size of received data
	{
	  len = recvfrom(s,data_buf,len,(uint8*)destip,&destport);  // receive data from a destination
#ifdef DEBUG_PRINT	  
	  printf("CH %bu: Received %d bytes from %.3bu. %.3bu. %.3bu. %.3bu \r\n", 
	  		 s, len, destip[0],destip[1],destip[2],destip[3]);
#endif	  
	  if(len != sendto(s,data_buf,len,(uint8*)destip,destport))  // send the data to the destination
      {
	    	printf("Sendto Fail\r\n");
      }
#ifdef DEBUG_PRINT
	  printf("CH %bu: Send %d bytes to Peer \r\n", s, len);
#endif

	}
	break;
	                                                    // -----------------
    case SOCK_CLOSED:                                  // CLOSED
      close(s);                                       // close the SOCKET
      socket(s,Sn_MR_UDP,port,mode);                  // open the SOCKET with UDP mode
      printf("LOOPBACK_UDPStarted. CH : %bu\r\n",s);
	  break;
    
	default:
      break;
  }
}