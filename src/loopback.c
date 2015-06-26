#include "../Drivers/TCPIPcore.h"	 
#include "loopback.h"

#define DEBUG_PRINT
#define HMI_FPGA_MAX_RECV_SIZE 255          /*FPGA接收缓存只接收255字节大小的帧*/

/*
 功能描述    : 将数据接收上来并传输到FPGA缓存当中
 参数        : @s socket number
               @data_buf W7100A指向的数据存储位置
 日期        : 2015年6月26日 21:04:42
*/
static void loopback_recv(SOCKET s)
{
    uint16 xdata len = 0;
    uint8 xdata destip[4] = { 0 };
    uint16 xdata destport = 0;

    if ((len = getSn_RX_RSR(s)) > 0)                   // check the size of received data
    {
        /*将数据拷贝到fpga的空间当中*/
        len = recvfrom(s, 0x010000, len, (uint8*)destip, &destport);

#ifdef DEBUG_PRINT	  
        printf("CH %bu: Received %d bytes from %.3bu. %.3bu. %.3bu. %.3bu \r\n",
            s, len, destip[0], destip[1], destip[2], destip[3]);
#endif
    }
}

/*
 功能描述    : 从FPGA当中读取数据并通过IP核发送出去
 参数        : 无
 日期        : 2015年6月26日 21:05:19
*/
static void loopback_send(SOCKET s)
{
    uint8 xdata b_has_data = 0;
    uint16 xdata data_len = 0;
    uint8 xdata destip[4] = { 0 };
    uint16 xdata destport = 0;

    wizmemcpy(0x13000, (unsigned long)&data_len, 2);     /*数据长度*/
    wizmemcpy(0x13002, (unsigned long)&b_has_data, 1);   /*是否有数标志*/

    if (0x01 == b_has_data)
    {
        wizmemcpy(0x013006, (unsigned long)&destip, 4);
        wizmemcpy(0x01300a, (unsigned long)&destport, 2);

#ifdef DEBUG_PRINT
        printf("pc104 send data to %d.%d.%d.%d:%d,size:%d\n",
            destip[0],destip[1],destip[2],destip[3],
            destport,
            data_len
            );
#endif /*DEBUG_PRINT*/

        /*这里本应该检查一次data_len的长度，但是发现sendto当中会检查一次，所以忽略*/
        sendto(s, 0x011000, data_len, (uint8*)destip, destport);

        b_has_data = 0;
        /*拷贝完数据，将有数标志重置为0*/
        wizmemcpy((unsigned long)&b_has_data, 0x13002, 1);
    }
}

void loopback_udp(SOCKET s, uint16 port, uint8 xdata *data_buf, uint16 mode)
{
    switch (IINCHIP_READ(Sn_SR(s)))
    {
    case SOCK_UDP:
        loopback_recv(s);
        loopback_send(s);
        break;
    case SOCK_CLOSED:                                  // CLOSED
        close(s);                                       // close the SOCKET
        socket(s, Sn_MR_UDP, port, mode);                  // open the SOCKET with UDP mode
        printf("LOOPBACK_UDPStarted. CH : %bu\r\n", s);
        break;
    default:
        break;
    }
}