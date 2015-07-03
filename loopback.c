#include <stdio.h>
#include "serial.h"
#include "types.h"
#include "loopback.h"
#include "TCPIPcore.h"
#include "socket.h"
#include "wizmemcpy.h"
#include "w7100.h"
/*
uint8 xdata * data_buf_A_flag=(uint8 xdata *)0x013002;
uint8 xdata * data_buf_B_flag=(uint8 xdata *)0x013005;
uint8 xdata * data_buf_A_len0=(uint8 xdata *)0x013000;
uint8 xdata * data_buf_A_len1=(uint8 xdata *)0x013001;
uint8 xdata * data_buf_B_len0=(uint8 xdata *)0x013003;
uint8 xdata * data_buf_B_len1=(uint8 xdata *)0x013004;
uint16 xdata * data_A=(uint16 xdata *)0x011000;
uint16 xdata * data_B=(uint16 xdata *)0x012000;
*/
bit Sock0_TimeOut, Sock0_Recv, Sock0_SendOk;
/*---------------------------------------------------------------------------*/
/*                      TCPIP Core Interrupt Handler                         */
/*---------------------------------------------------------------------------*/
void loopback_udp(SOCKET s, uint16 port, uint8 xdata * data_buf, uint16 mode)
{
  uint16 xdata len=32; 
  uint8 xdata destip[4];//={192,168,1,3};
  uint16 xdata destport;//=3000;
  uint8 temp =0;
 // uint16 xdata data_A,data_B;
 // uint8 xdata data__A_flag,data_B_flag;
 // uint8 xdata data_A_len0,data_A_len1,data_B_len0,data_B_len1;
  while(IINCHIP_READ(Sn_SR(s))!=SOCK_UDP)
  {
  	close(s);                                       // close the SOCKET
  	socket(s,Sn_MR_UDP,port,mode); //Init UDP sets
  }
  if (getSn_IR(s) & Sn_IR_RECV)
  {
  	setSn_IR(s, Sn_IR_RECV);
	len=getSn_RX_RSR(s);
	len = recvfrom(s,data_buf,len,(uint8*)destip,&destport);  // receive data from a destination
	///////////////////////////////////add data_buf handle function///////////////////////////
	//when it is need to send data from FPGA to W7100A,data_buf should send a"ask for":0xffff,however 0x55AA is the frame flag

	wizmemcpy((0x000000+(uint16)(data_buf)),0x010000,len);	//要保证上次数据FPGA已经读完
	P3_0=0x00;
	P3_0=0x01;					   

  }
//	wizmemcpy((0x000000+(uint16)data_buf),0x010040,11);
//	wizmemcpy(0x010050,(0x000000+(uint16)data_buf),11);
	wizmemcpy(0x013002,0x000000+(uint16)(data_buf+2),1); //==1 A有数据
  	wizmemcpy(0x013005,0x000000+(uint16)(data_buf+5),1); //==1 B有数据
  	wizmemcpy(0x013000,0x000000+(uint16)data_buf,1);	 //A数据长度
  	wizmemcpy(0x013001,0x000000+(uint16)(data_buf+1),1);
  	wizmemcpy(0x013003,0x000000+(uint16)(data_buf+3),1);  //B数据长度
  	wizmemcpy(0x013004,0x000000+(uint16)(data_buf+4),1);
	//B_flag=*(data_buf+5);
	//len_B=(*(data_buf+3))<<8+*(data_buf+4);

	if((*(data_buf+2))==1) //A
	{
		*(data_buf+2)=0;
		wizmemcpy(0x000000+(uint16)(data_buf+2),0x013002,1);
		len=(*(data_buf))+((*(data_buf+1))<<8);
		
		if(len>0)
		{
		//wizmemcpy(0x011000,data_buf,11);
		//////////////////////////////////////////////////////////
		wizmemcpy(0x013006,0x000000+(uint16)(data_buf),1);//192
		wizmemcpy(0x013007,0x000000+(uint16)(data_buf+1),1);//168
		wizmemcpy(0x013008,0x000000+(uint16)(data_buf+2),1);
		wizmemcpy(0x013009,0x000000+(uint16)(data_buf+3),1);
		wizmemcpy(0x01300a,0x000000+(uint16)(data_buf+4),1);	//IP%PORT
		wizmemcpy(0x01300b,0x000000+(uint16)(data_buf+5),1);
		destip[0]=*(data_buf);
		destip[1]=*(data_buf+1);
		destip[2]=*(data_buf+2);
		destip[3]=*(data_buf+3);
		destport=(*(data_buf+4)<<8)+*(data_buf+5);
		//////////////////////////////////////////////////////////
		wizmemcpy(0x011000,(0x000000+(uint16)data_buf),len);
		sendto(s,data_buf,len,(uint8*)destip,destport);
		}	
	}
	else if((*(data_buf+5))==1/*B_flag==1*/)	 //B
	{
		*(data_buf+5)=0;
		wizmemcpy(0x000000+(uint16)(data_buf+5),0x013005,1);
		//len=len_B;
		len=(*(data_buf+3))+((*(data_buf+4))<<8);
		if(len>0)
		{
		//////////////////////////////////////////////////////////
		wizmemcpy(0x01300c,0x000000+(uint16)(data_buf),1);
		wizmemcpy(0x01300d,0x000000+(uint16)(data_buf+1),1);
		wizmemcpy(0x01300e,0x000000+(uint16)(data_buf+2),1);
		wizmemcpy(0x01300f,0x000000+(uint16)(data_buf+3),1);
		wizmemcpy(0x013010,0x000000+(uint16)(data_buf+4),1);	//IP%PORT
		wizmemcpy(0x013011,0x000000+(uint16)(data_buf+5),1);
		destip[0]=*(data_buf);
		destip[1]=*(data_buf+1);
		destip[2]=*(data_buf+2);
		destip[3]=*(data_buf+3);
		destport=(*(data_buf+4)<<8)+*(data_buf+5);
		//////////////////////////////////////////////////////////
		wizmemcpy(0x012000,(0x000000+(uint16)data_buf),len);
		sendto(s,data_buf,len,(uint8*)destip,destport);
		}	
	}
 	else
	{}
}