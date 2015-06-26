/**********************************************
(c)COPYRIGHT WIZnet Inc. ALL RIGHT RESERVED
 W7100 driver
**********************************************/
/*
*
@file		TCPIPCore.c
@brief	define functions to use WIZnet TCPIPCore
*
*/

#include "TCPIPCore.h"
																								
vuint8 xdata I_STATUS[MAX_SOCK_NUM] = {0,};
uint16 xdata SMASK[MAX_SOCK_NUM]; /**< Variable for Tx buffer MASK in each channel */
uint16 xdata RMASK[MAX_SOCK_NUM]; /**< Variable for Rx buffer MASK in each channel */
uint16 xdata SSIZE[MAX_SOCK_NUM]; /**< Max Tx buffer size by each channel */
uint16 xdata RSIZE[MAX_SOCK_NUM]; /**< Max Rx buffer size by each channel */
uint16 xdata SBUFBASEADDRESS[MAX_SOCK_NUM]; /**< Tx buffer base address by each channel */
uint16 xdata RBUFBASEADDRESS[MAX_SOCK_NUM]; /**< Rx buffer base address by each channel */

static uint8 xdata SUBNET[4];

/*	
********************************************************************************
*              MCU Core Initialization Function
*
* Description: MCU Core Initialization
* Arguments  : None.
* Returns    : None.
* Note       : 
********************************************************************************
*/			
void  Init_iMCU(void)
{ 
	uint8 tmpEA;

	CHIP_Init(); // MCU Core init

	tmpEA = EA;
	EA = 0; 		// Disable Global interrupts

	CKCON = 0x02;		// External Memory Access Time
	WTST = 0x04;	   // Code Memory Wait State
#ifdef __DEF_IINCHIP_INT__	
	uint8 xdata i;
	IINCHIP_WRITE(IMR,0xff);
	for(i=0;i<MAX_SOCK_NUM;i++) IINCHIP_WRITE(Sn_IMR(i),0x1f);	
	IINCHIP_ISR_ENABLE();
#endif

#ifdef USE_QFN64	
	PHY_mode_config();	// PHY setting of W7100A, this function is just for QFN 64pin package
						// Default is "000" = auto-negotiation mode
#endif
	
	InitSerial(); 	// Initialize serial port (Refer to serial.c)

	P0_PU = 0xFF;	// GPIO Pull-up setting for 3.3V output     
	P1_PU = 0xFF;
	P2_PU = 0xFF;
	P3_PU = 0xFF;
	
	EA = tmpEA;
}											
	   										
/*
********************************************************************************
*              Network parameter Initialization Function
*
* Description: Network information initialzation
* Arguments  : None.
* Returns    : None.
* Note       : HTTP Server example does not use this function.
********************************************************************************
*/
void Init_Network(void)
{				  
	/* Network information setting */
	
	uint8 xdata ip[4] = {192,168,1,2};                   	// for setting SIP register
	uint8 xdata gw[4] = {192,168,1,1};                     	// for setting GAR register
	uint8 xdata sn[4] = {255,255,255,0};                    // for setting SUBR register			
	uint8 xdata mac[6] = {0x00,0x08,0xDC,0x11,0x01,0x02};   // for setting SHAR register

	/* Assign the TX, RX memory of each SOCKETs */	 
	uint8 xdata txsize[MAX_SOCK_NUM] = {8,2,1,1,1,1,1,1};
	uint8 xdata rxsize[MAX_SOCK_NUM] = {8,2,1,1,1,1,1,1};
	//uint8 xdata txsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};
	//uint8 xdata rxsize[MAX_SOCK_NUM] = {2,2,2,2,2,2,2,2};

	uint8 xdata str[17];
	uint8 xdata rsn[4];
	SetSubnet(sn);	   	

	/* Write IP address of W7100A to SIPR register */
	IINCHIP_WRITE (SIPR0+0,ip[0]);
	IINCHIP_WRITE (SIPR0+1,ip[1]);
	IINCHIP_WRITE (SIPR0+2,ip[2]);
	IINCHIP_WRITE (SIPR0+3,ip[3]);
	
	/* Write GATEWAY address of W7100A to GAR register */
	IINCHIP_WRITE (GAR0+0, gw[0]);
	IINCHIP_WRITE (GAR0+1, gw[1]);
	IINCHIP_WRITE (GAR0+2, gw[2]);
	IINCHIP_WRITE (GAR0+3, gw[3]);

	/* Write SUBNETMASK of W7100A to SURR register */
	ClearSubnet();

	/* Write MAC address of W7100A to SHAR register */
	IINCHIP_WRITE (SHAR0+0, mac[0]);
	IINCHIP_WRITE (SHAR0+1, mac[1]);
	IINCHIP_WRITE (SHAR0+2, mac[2]);
	IINCHIP_WRITE (SHAR0+3, mac[3]);
	IINCHIP_WRITE (SHAR0+4, mac[4]);
	IINCHIP_WRITE (SHAR0+5, mac[5]);

  	set_MEMsize(txsize,rxsize);

	/*If you use sotfware reset(such as watchdog reset), 
	please don't use this code or you may can't escape this while loop!!*/
	//while(!(EIF & 0x02));
	//EIF &= ~0x02;

	printf( "\r\n==============================================\r\n");
	printf( "       W7100A  Net Config Information         \r\n");
	printf( "==============================================\r\n");

	printf( "MAC ADDRESS IP : %.2bx.%.2bx.%.2bx.%.2bx.%.2bx.%.2bx\r\n", 
		 IINCHIP_READ (SHAR0+0),  IINCHIP_READ (SHAR0+1), 
		 IINCHIP_READ (SHAR0+2),  IINCHIP_READ (SHAR0+3),
		 IINCHIP_READ (SHAR0+4),  IINCHIP_READ (SHAR0+5)
		) ;
 	
	GetSubnet(rsn);
	sprintf(str,"%.3bu.%.3bu.%.3bu.%.3bu",
			 rsn[0],  rsn[1], 
			 rsn[2],  rsn[3]);

	printf( "SUBNET MASK  : %s\r\n",str); 
 	
	sprintf(str,"%.3bu.%.3bu.%.3bu.%.3bu",
			 IINCHIP_READ (GAR0+0),  IINCHIP_READ (GAR0+1), 
			 IINCHIP_READ (GAR0+2),  IINCHIP_READ (GAR0+3));

	printf( "G/W IP ADDRESS : %s\r\n",str);

 	sprintf(str,"%.3bu.%.3bu.%.3bu.%.3bu",
			 IINCHIP_READ (SIPR0+0),  IINCHIP_READ (SIPR0+1), 
			 IINCHIP_READ (SIPR0+2),  IINCHIP_READ (SIPR0+3));

	printf( "LOCAL IP ADDRESS : %s\r\n", str);	
}

/*********************************************************************** 
@brief	W7100/W7100A MCU Core initialization function
@input	Non
@output Non
***********************************************************************/
void CHIP_Init(void)
{
	WCONF = 0x40;		// Clear the RB bit to prevent abnormal running
	//CKCON = 0x02;		//	Data memory wait state setting
	//WTST = 0x04;		//	Code memory access time	
}


/*********************************************************************** 
@brief	PHY mode configuration function for W7100A.	The 64pin package of 
		W7100A don't have mode[2:0] pins to configure the PHY mode.
		So QFN 64pin package of W7100A must use this function on its
		initialize function.
		The LQFP 100pin package can configure the PHY mode not only 
		using the PHY mode config pins but also using this function.

@input	Non
@output Non
***********************************************************************/
void PHY_mode_config(void)
{
	PHYCONF = 0x08;	//MODE_EN enable to use SFR and set the mode[2:0] bit to "000" = auto-negotiation	
	PHYCONF |= 0x20; // Reset bit set, need to be reset so that W7100A get the mode[2:0] value and setup
	wait_1ms(100);	// Delay for reset timing
	PHYCONF &= ~(0x20);	 // Reset bit clear
}


/****************************************************************************
@brief	This function writes the data into W7100/W7100A register/memory area
@input	addr: address of W7100 register or memory space
				this address is an absolute address.
				The base address is defined to "COMMON_BASE" in iinchip_conf.h
				wizdata: 8bit data to write to W7100/W7100A
@output	Non
****************************************************************************/
void IINCHIP_WRITE(uint16 addr,uint8 wizdata) 
{
	uint8 tmpEA;

	tmpEA = EA;
	EA = 0;
	DPX0 = 0xFE;
	*((vuint8 xdata*)(addr)) = wizdata;
	DPX0 = 0x00;
	EA = tmpEA;
}


/****************************************************************************
@brief	This function reads the data from W7100/W7100A register/memory area
@input	addr: address of W7100 register or memory space
				this address is an absolute address.
				The base address is defined to "COMMON_BASE" in iinchip_conf.h
@output	8bit data from W7100/W7100A
****************************************************************************/
uint8 IINCHIP_READ(uint16 addr)
{
	register wizdata;
	uint8 tmpEA;

	tmpEA = EA;
	EA = 0;
	DPX0 = 0xFE;
	wizdata = *((vuint8 xdata*)(addr));
	DPX0 = 0x00;
	EA = tmpEA;
	return wizdata;
}


/****************************************************************************
@brief	Socket interrupt (TCPIPCore interrupt) service routine
				Save the interrupt value and clear it.
@input 	Non
@output Non
****************************************************************************/
#ifdef __DEF_IINCHIP_INT__ 
static void iinchip_irq(void) interrupt 11
{
	uint8 int_val;
	uint8 i;
	IINCHIP_ISR_DISABLE();

	int_val = IINCHIP_READ(IR);

	if (int_val & IR_CONFLICT)
	{
		printf("IP conflict : %.2x\r\n", int_val);
	}
	/*
	if (int_val & IR_UNREACH)
	{
		printf("INT Port Unreachable : %.2x\r\n", int_val);
		printf("UIPR0 : %d.%d.%d.%d\r\n", IINCHIP_READ(UIPR0), IINCHIP_READ(UIPR0+1), IINCHIP_READ(UIPR0+2), IINCHIP_READ(UIPR0+3));
		printf("UPORT0 : %.2x %.2x\r\n", IINCHIP_READ(UPORT0), IINCHIP_READ(UPORT0+1));
	}
	*/
	IINCHIP_WRITE(IR, int_val);
	int_val = IINCHIP_READ(IR2);
	for (i=0; i<MAX_SOCK_NUM; i++) 
	{
    if (int_val & IR_SOCK(i)) 
		{
         		I_STATUS[i] |= IINCHIP_READ(Sn_IR(i)); 
            IINCHIP_WRITE(Sn_IR(i), I_STATUS[i]);
		}
	}
	IINCHIP_WRITE(IR2, int_val);
	EIF &= 0xF7;	// Clear TCPIPCore Interrupt Flag	
	IINCHIP_ISR_ENABLE();
}
#endif


/****************************************************************************
@brief	Just return the interrupt value saved from interrupt service routine
@input 	s: socket number
@output interrupt value of related socket
****************************************************************************/
uint8 getSn_IR(uint8 s)
{
#ifdef __DEF_IINCHIP_INT__
	return I_STATUS[s];
#else
	return IINCHIP_READ(Sn_IR(s));
#endif
}


/****************************************************************************
@brief	Set the interrupt value, in general this function is used for
		clearing the interrupt.
		Because we have to write same value to clear the interrupt.
@input 	s: socket number
@output interrupt value of related socket
****************************************************************************/
void setSn_IR(uint8 s, uint8 val)
{
#ifdef __DEF_IINCHIP_INT__
	I_STATUS[s] = I_STATUS[s] & ~(val);		//Modified v1.3 Before: I_STATUS[s] = val;
#else
	IINCHIP_WRITE(Sn_IR(s), val);
#endif
}


/*****************************************************************************
@brief	This function set the transmit & receive buffer size as per the 
		channels is used Maximum memory size for Tx, Rx in the W7100 
		is 16K Bytes, in the range of 16KBytes, the memory size could be 
		allocated dynamically by each channel.
		Be attentive to sum of memory size shouldn't exceed 16Kbytes
		and to data transmission and receiption from non-allocated channel 
		may cause some problems.
		If the 16KBytes memory is already  assigned to centain channel,
		other channels couldn't be used, for there's no available memory.

@input	tx_size: array pointer of TX memory size. 
		 ex> tx_size = {2,2,2,2,2,2,2,2}, all tx memory of each sockets are 2KB
		rx_size: array pointer of RX memory size. 				
		 ex> tx_size = {2,2,2,2,2,2,2,2}, all rx memory of each sockets are 2KB
@output Non
*****************************************************************************/ 
void set_MEMsize(	uint8* tx_size, uint8* rx_size)
{
	int16 xdata i;
	int16 xdata ssum,rsum;

	ssum = 0;
	rsum = 0;
	
	SBUFBASEADDRESS[0] = (uint16)(__DEF_IINCHIP_MAP_TXBUF__);		/* Set base address of Tx memory for channel #0 */
	RBUFBASEADDRESS[0] = (uint16)(__DEF_IINCHIP_MAP_RXBUF__);		/* Set base address of Rx memory for channel #0 */

	for (i = 0 ; i < MAX_SOCK_NUM; i++)       // Set the size, masking and base address of Tx & Rx memory by each channel
	{
		IINCHIP_WRITE((Sn_TXMEM_SIZE(i)),tx_size[i]);
		IINCHIP_WRITE((Sn_RXMEM_SIZE(i)),rx_size[i]);

		SSIZE[i] = (int16)(0);
		RSIZE[i] = (int16)(0);

		if (ssum < 16*1024)
		{
	    switch( tx_size[i] )
			{
			case 1:
				SSIZE[i] = (int16)(1024);
				SMASK[i] = (uint16)(0x03FF);
				break;
			case 2:
				SSIZE[i] = (int16)(2048);
				SMASK[i] = (uint16)(0x07FF);
				break;
			case 4:
				SSIZE[i] = (int16)(4096);
				SMASK[i] = (uint16)(0x0FFF);
				break;
			case 8:
				SSIZE[i] = (int16)(8192);
				SMASK[i] = (uint16)(0x1FFF);
				break;
			case 16:
				SSIZE[i] = (int16)(16384);
				SMASK[i] = (uint16)(0x3FFF);
				break;			  
			}
		}

		if (rsum < 16*1024)
		{
      switch( rx_size[i] )
			{
			case 1:
				RSIZE[i] = (int16)(1024);
				RMASK[i] = (uint16)(0x03FF);
				break;
			case 2:
				RSIZE[i] = (int16)(2048);
				RMASK[i] = (uint16)(0x07FF);
				break;
			case 4:
				RSIZE[i] = (int16)(4096);
				RMASK[i] = (uint16)(0x0FFF);
				break;
			case 8:
				RSIZE[i] = (int16)(8192);
				RMASK[i] = (uint16)(0x1FFF);
				break;
			case 16:
				RSIZE[i] = (int16)(16384);
				RMASK[i] = (uint16)(0x3FFF);
				break;
			}
		}
		ssum += SSIZE[i];
		rsum += RSIZE[i];

    if (i != 0)             // Sets base address of Tx and Rx memory for channel #1,#2,#3
		{
			SBUFBASEADDRESS[i] = SBUFBASEADDRESS[i-1] + SSIZE[i-1];
			RBUFBASEADDRESS[i] = RBUFBASEADDRESS[i-1] + RSIZE[i-1];
		}
	}
}


/*****************************************************************************
@brief	get the value of socket status register. This value is used for the
		state machine. We can do some work according to this value.
@input	s: socket number
@output	the value of Sn_SR(s)
*****************************************************************************/
uint8 getSn_SR(SOCKET s)
{
	uint8 xdata sr=0xff, sr1=0xff;

	while(1)
	{
		sr = (uint8)IINCHIP_READ(Sn_SR(s)); 
		if(sr == sr1) break;                 
		sr1 = sr;                            
	}
	return sr;
}


/*****************************************************************************
@brief	get socket TX free buf size. This function gives free buffer size of 
		transmit buffer. This is the data size that user can transmit.
		User shuold check this value first and control the size of transmitting data.
@input	s: socket number
@output	the value of Sn_TX_FSR(s)
*****************************************************************************/
uint16 getSn_TX_FSR(SOCKET s)
{
	uint16 xdata val=0;
 	uint16 xdata val1=0;
   
	do
	{
			val1 = IINCHIP_READ(Sn_TX_FSR0(s));
			val1 = (val1 << 8) + IINCHIP_READ(Sn_TX_FSR0(s) + 1);
		  if (val1 != 0)
			{
	   			val = IINCHIP_READ(Sn_TX_FSR0(s));
	   			val = (val << 8) + IINCHIP_READ(Sn_TX_FSR0(s) + 1);
			}
	} while (val != val1);	// Double check the read data was correct.
  return val;
}


/*****************************************************************************
@brief	get socket RX recv buf size. This gives size of received data in 
		receive buffer. We can read the received data as many as this size.
@input	s: socket number
@output	the value of Sn_RX_RSR(s)
*****************************************************************************/
uint16 getSn_RX_RSR(SOCKET s)
{
	uint16 xdata val=0;
	uint16 xdata val1=0;

	do
	{
		val1 = IINCHIP_READ(Sn_RX_RSR0(s));
		val1 = (val1 << 8) + IINCHIP_READ(Sn_RX_RSR0(s) + 1);
        if(val1 != 0)
		{
   			val = IINCHIP_READ(Sn_RX_RSR0(s));
   			val = (val << 8) + IINCHIP_READ(Sn_RX_RSR0(s) + 1);
		}
	} while (val != val1);
    return val;
}


/*****************************************************************************
@brief	A memory copy function from normal memory => to TCPIPCore TX memory
		This function is being called by send() and sendto() function also.
		This function read the Tx write pointer register and after copy the 
		data in buffer update the Tx write pointer register. 
		User should read upper byte first and lower byte later to get proper value.
@input	s: socket number
		src: source address
		dst: destination address
		len: data length to be copied
		A memory copy from src to dst as many as len
@output	Non
*****************************************************************************/
void IINCHIP_WRITE_BUF(SOCKET s, vuint8 * src, vuint8 * dst, uint16 len)
{
	uint16 xdata size;
	uint16 xdata dst_mask;
	uint16 xdata src_ptr = (uint16)src;
	uint16 xdata dst_ptr;
	
	dst_mask = (uint16)dst & SMASK[s];
	dst_ptr = SBUFBASEADDRESS[s] + dst_mask;

	if (dst_mask + len > SSIZE[s]) 
	{
		size = SSIZE[s] - dst_mask;
		wizmemcpy((0x000000l+src_ptr), (0xFE0000l+dst_ptr),size);
		src_ptr += size;
		size = len - size;
		dst_ptr = SBUFBASEADDRESS[s];
		wizmemcpy((0x000000l+src_ptr),(0xFE0000l+dst_ptr),size);
	} 
	else wizmemcpy((0x000000l+src_ptr),(0xFE0000l+dst_ptr),len);
}



/*****************************************************************************
@brief	A memory copy function from TCPIPCore RX memory => to normal memory 
		This function is being called by recv() also. This function read the 
		Rx read pointer register	and after copy the data from receive buffer 
		update the Rx write pointer register. User should read upper byte 
		first and lower byte later to get proper value.
@input	s: socket number
		src: source address
		dst: destination address
		len: data length to be copied
		A memory copy from src to dst as many as len
@output	Non
*****************************************************************************/
void IINCHIP_READ_BUF(SOCKET s, vuint8 * src, uint8 * dst, uint16 len)
{
	uint16 xdata size;
	uint16 xdata src_mask;
	uint16 xdata src_ptr;
	uint16 xdata dst_ptr = (uint16)dst;
	
	
	src_mask = (uint16)src & RMASK[s];
	src_ptr = RBUFBASEADDRESS[s] + src_mask;

	if( (src_mask + len) > RSIZE[s] ) 
	{
		size = RSIZE[s] - src_mask;
		wizmemcpy((0xFE0000l+src_ptr),(0x000000l+dst_ptr),size);
		dst_ptr += size;
		size = len - size;
		src_ptr = RBUFBASEADDRESS[s];
		wizmemcpy((0xFE0000l+src_ptr),(0x000000l+dst_ptr),size);
	} 
	else
	{
		wizmemcpy((0xFE0000l+src_ptr),(0x000000l+dst_ptr),len);
	}
	
}



#ifdef __DEF_IINCHIP_PPP__			// PPPoE Driver for W7100A
#define PPP_OPTION_BUF_LEN 64
uint8 pppinit_in(uint8 * id, uint8 idlen, uint8 * passwd, uint8 passwdlen) large;


/******************************************************************************
@brief	make PPPoE connection using W7100/W7100A
@input	id: Identifier
		idlen: The length of ID
		passwd: password for ID
		passwdlen: The length of password
@output	1: success to connect
		2: Auth fail
		3: timeout
		4: Auth type not support
******************************************************************************/
uint8 pppinit(uint8 * id, uint8 idlen, uint8 * passwd, uint8 passwdlen) large
{
	uint8 xdata ret;

	// PHASE0. PPPoE(ADSL) setup
#ifdef __DEF_PPP_DBG__
   printf("PHASE 0. PPPoE setup\r\n");
#endif
	IINCHIP_WRITE(MR,IINCHIP_READ(MR) | MR_PPPOE);	// enable pppoe mode

	// Open socket in pppoe mode
   setSn_IR(0, getSn_IR(0));     // first clear isr(0), W3150A+ at present time
	IINCHIP_WRITE(PTIMER,200);    // 5sec timeout
	IINCHIP_WRITE(PMAGIC,0x01);   // magic number
	IINCHIP_WRITE(Sn_MR(0),Sn_MR_PPPOE);
	IINCHIP_WRITE(Sn_CR(0),Sn_CR_OPEN);
	while( IINCHIP_READ(Sn_CR(0)) );

	ret = pppinit_in(id, idlen, passwd, passwdlen);

/*	
// close ppp connection socket
	IINCHIP_WRITE(Sn_CR(0),Sn_CR_CLOSE);
	while( IINCHIP_READ(Sn_CR(0)) );
	setSn_IR(0, 0xFF);	// added VER_3_7, all interrupt clear
*/	
	if(ret != PPP_RET_SUCCESS) 
	   IINCHIP_WRITE(MR,IINCHIP_READ(MR) & ~MR_PPPOE);
	return ret;
}


/******************************************************************************
@brief	Subfunction of pppinit
@input	id: Identifier
		idlen: The length of ID
		passwd: password for ID
		passwdlen: The length of password
@output	1: success to connect
		2: Auth fail
		3: timeout
		4: Auth type not support
******************************************************************************/
uint8 pppinit_in(uint8 * id, uint8 idlen, uint8 * passwd, uint8 passwdlen)	large
{
	uint8  xdata loop_idx = 0;
	uint8  xdata isr = 0;
	uint8  xdata buf[PPP_OPTION_BUF_LEN];	// enable VER_3_7
	uint16 xdata len;
	uint8  xdata str[PPP_OPTION_BUF_LEN];	// enable VER_3_7
	uint8  xdata str_idx, xdata dst_idx;
	uint16 xdata ptr = 0;

#ifdef __DEF_PPP_DBG__
   uint16 i = 0;
#endif  
	// PHASE1. PPPoE Discovery
	// start to connect pppoe connection
#ifdef __DEF_PPP_DBG__
   printf("PHASE 1. PPPoE Discovery..");
#endif		
	IINCHIP_WRITE(Sn_CR(0),Sn_CR_PCON);
	while( IINCHIP_READ(Sn_CR(0)) );
	wait_10ms(100);
#ifdef __DEF_PPP_DBG__
   printf("ok\r\n");
#endif   
	
	loop_idx = 0;
	//check whether PPPoE discovery end or not
	while (!(getSn_IR(0) & Sn_IR_PNEXT))
	{
#ifdef __DEF_PPP_DBG__
      printf(".");
#endif      
		if (loop_idx++ == 10) // timeout
		{
#ifdef __DEF_PPP_DBG__
         printf("timeout before LCP\r\n");
#endif   
			return PPP_RET_ERR_DISCOVERY;
		}
		wait_10ms(100);
	}
	setSn_IR(0, 0xFF);	//added VER_3_7, all interrupt clear

	// PHASE2. LCP process
#ifdef __DEF_PPP_DBG__
   printf("PHASE 2. LCP");
#endif   

	// send LCP Request
	// Magic number option
	// option format (type value + length value + data)
	// write magic number value
	buf[0] = 0x05; // type value
	buf[1] = 0x06; // length value
	buf[2] = 0x01;
	buf[3] = 0x01;
	buf[4] = 0x01;
	buf[5] = 0x01; // data
	// for MRU option, 1492 0x05d4
	// buf[6] = 0x01; buf[7] = 0x04; buf[8] = 0x05; buf[9] = 0xD4;

	// read TX write pointer
	ptr = IINCHIP_READ(Sn_TX_WR0(0));
	ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ(Sn_TX_WR0(0) + 1);

   // copy data
	IINCHIP_WRITE_BUF(0, buf, (uint8*)ptr, 0x06);

	// Update TX write pointer
	ptr += 0x06;
	IINCHIP_WRITE(Sn_TX_WR0(0),(uint8)((ptr & 0xff00) >> 8));
	IINCHIP_WRITE((Sn_TX_WR0(0) + 1),(uint8)(ptr & 0x00ff));
	
	IINCHIP_WRITE(Sn_CR(0),Sn_CR_PCR); // send request
	while( IINCHIP_READ(Sn_CR(0)) );
	wait_10ms(100);

   loop_idx = 0;
	while (!((isr = getSn_IR(0)) & Sn_IR_PNEXT))
	{
		if (isr & Sn_IR_PRECV) // Not support option
		{
#ifdef __DEF_PPP_DBG__
         printf("Not support option\r\n");
#endif		   
			IINCHIP_WRITE(Sn_IR(0), Sn_IR_PRECV); //added VER_3_7, clear interrupt value
			len = getSn_RX_RSR(0);
			if ( len > 0 )
			{
				// read RX read pointer
				ptr = IINCHIP_READ(Sn_RX_RD0(0));
				ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ(Sn_RX_RD0(0) + 1);

				// copy data
				IINCHIP_READ_BUF(0,(uint8*)ptr,str, len);

				// Update RX read pointer
				ptr += len;
				IINCHIP_WRITE(Sn_RX_RD0(0),(uint8)((ptr & 0xff00) >> 8));
				IINCHIP_WRITE((Sn_RX_RD0(0) + 1),(uint8)(ptr & 0x00ff));

				IINCHIP_WRITE(Sn_CR(0),Sn_CR_RECV);
				while( IINCHIP_READ(Sn_CR(0)) );
				// for debug
#ifdef __DEF_PPP_DBG__				
				printf("LCP proc\r\n"); for (i = 0; i < len; i++) printf ("%02bx ", str[i]); printf("\r\n");
#endif				
				// get option length
				len = str[4];
				len = ((len & 0x00ff) << 8) + str[5];
				len += 2;
				str_idx = 6;
				dst_idx = 0; // ppp header is 6 byte, so starts at 6.
				do
				{
					if ((str[str_idx] == 0x01) || (str[str_idx] == 0x02) || (str[str_idx] == 0x03) || (str[str_idx] == 0x05))
					{
						// skip as length of support option. str_idx+1 is option's length.
						str_idx += str[str_idx+1];
					}
					else
					{
						// not support option , REJECT
						memcpy((uint8 *)(buf+dst_idx), (uint8 *)(str+str_idx), str[str_idx+1]);
						dst_idx += str[str_idx+1];
						str_idx += str[str_idx+1];
					}
#ifdef __DEF_PPP_DBG__					
					printf("s: %u, d: %u, l: %u\r\n", str_idx, dst_idx, len);
#endif					
				}while (str_idx != len);
#ifdef __DEF_PPP_DBG__				
				printf("LCP dst proc\r\n"); for (i = 0; i < dst_idx; i++) printf ("%02bx ", buf[i]); printf("\r\n");
#endif
				// send LCP REJECT packet
				// read TX write pointer
				ptr = IINCHIP_READ(Sn_TX_WR0(0));
				ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ(Sn_TX_WR0(0) + 1);

            // copy data
				IINCHIP_WRITE_BUF(0, buf, (uint8*)ptr, dst_idx);

				// Update TX write pointer
				ptr += dst_idx;
				IINCHIP_WRITE(Sn_TX_WR0(0),(uint8)((ptr & 0xff00) >> 8));
				IINCHIP_WRITE((Sn_TX_WR0(0) + 1),(uint8)(ptr & 0x00ff));
	
				IINCHIP_WRITE(Sn_CR(0),Sn_CR_PCJ);
				while( IINCHIP_READ(Sn_CR(0)) );
#ifdef __DEF_PPP_DBG__				
            printf("SEND REJECT OPTION\r\n");
#endif				
			}
		}
#ifdef __DEF_PPP_DBG__
      printf(".");
#endif      
		if (loop_idx++ == 10) // timeout, modified value from 5 to 10 VER_3_7
		{
#ifdef __DEF_IINCHIP_DBG__
         printf("timeout after LCP\r\n");
#endif   
			return PPP_RET_ERR_LCP;
		}
		wait_10ms(100);
	}
	setSn_IR(0, 0xFF);	//added VER_3_7, all interrupt clear

	/* +200903[bj] clear receive buffer, added VER_3_7 */
	len = getSn_RX_RSR(0);
	if ( len > 0 )
	{
		// read RX read pointer
		ptr = IINCHIP_READ(Sn_RX_RD0(0));
		ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ(Sn_RX_RD0(0) + 1);

		// copy data
		IINCHIP_READ_BUF(0,(uint8*)ptr,str, len);

		// Update RX read pointer
		ptr += len;
		IINCHIP_WRITE(Sn_RX_RD0(0),(uint8)((ptr & 0xff00) >> 8));
		IINCHIP_WRITE((Sn_RX_RD0(0) + 1),(uint8)(ptr & 0x00ff));
#ifdef __DEF_PPP_DBG__				
		printf("dummy proc len = %u\r\n", len); for (i = 0; i < len; i++) printf ("%02bx ", str[i]); printf("\r\n");
#endif		
		IINCHIP_WRITE(Sn_CR(0),Sn_CR_RECV);
		while( IINCHIP_READ(Sn_CR(0)) );
	}
	 
#ifdef __DEF_PPP_DBG__
   printf(" ok\r\n");
	printf("PHASE 3. PPPoE Auth.mode\r\n");
	printf("Protocol:%02bx%02bx ",IINCHIP_READ(PATR0),IINCHIP_READ(PATR0+1));
#endif

	loop_idx = 0;
	if (IINCHIP_READ(PATR0) == 0xc0 && IINCHIP_READ(PATR0+1) == 0x23)
	{
#ifdef __DEF_IINCHIP_DBG__
      printf(":PAP\r\n"); // in case of adsl normally supports PAP.
      printf("id=%s, passwd=%s\r\n",id,passwd);
#endif
		// send authentication data
		// copy (idlen + id + passwdlen + passwd)
		buf[loop_idx] = idlen;
		loop_idx++;
		memcpy((uint8 *)(buf+loop_idx), (uint8 *)(id), idlen);
		loop_idx += idlen;
		buf[loop_idx] = passwdlen;
		loop_idx++;
		memcpy((uint8 *)(buf+loop_idx), (uint8 *)(passwd), passwdlen);
		loop_idx += passwdlen;

		// read TX write pointer
		ptr = IINCHIP_READ(Sn_TX_WR0(0));
		ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ(Sn_TX_WR0(0) + 1);

	      	// copy data
		IINCHIP_WRITE_BUF(0, buf, (uint8*)ptr, loop_idx);

		// Update TX write pointer
		ptr += loop_idx;
		IINCHIP_WRITE(Sn_TX_WR0(0),(uint8)((ptr & 0xff00) >> 8));
		IINCHIP_WRITE((Sn_TX_WR0(0) + 1),(uint8)(ptr & 0x00ff));
	
		IINCHIP_WRITE(Sn_CR(0),Sn_CR_PCR);
		while( IINCHIP_READ(Sn_CR(0)) );
	}
	else if (IINCHIP_READ(PATR0) == 0xc2 && IINCHIP_READ(PATR0+1) == 0x23)
	{
		uint8    xdata chal_len;
		md5_ctx  xdata context;
		uint8    xdata digest[16];

		len = getSn_RX_RSR(0);
		if ( len > 0 )
		{
			// read RX read pointer
			ptr = IINCHIP_READ(Sn_RX_RD0(0));
			ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ(Sn_RX_RD0(0) + 1);

			// copy data
			IINCHIP_READ_BUF(0,(uint8*)ptr,str, len);

			// Update RX read pointer
			ptr += len;
			IINCHIP_WRITE(Sn_RX_RD0(0),(uint8)((ptr & 0xff00) >> 8));
			IINCHIP_WRITE((Sn_RX_RD0(0) + 1),(uint8)(ptr & 0x00ff));

			IINCHIP_WRITE(Sn_CR(0),Sn_CR_RECV);
			while( IINCHIP_READ(Sn_CR(0)) );

// str is C2 23 xx CHAL_ID xx xx CHAP_LEN CHAP_DATA
// index  0  1  2  3       4  5  6        7 ...

			memset(buf,0x00,64);
			buf[loop_idx] = str[3];
			loop_idx++; // chal_id
			memcpy((uint8 *)(buf+loop_idx), (uint8 *)(passwd), passwdlen);
			loop_idx += passwdlen; //passwd
			chal_len = str[6]; // chal_id
			memcpy((uint8 *)(buf+loop_idx), (uint8 *)(str+7), chal_len);
			loop_idx += chal_len; //challenge
			buf[loop_idx] = 0x80;


			md5_init(&context);
			md5_update(&context, buf, loop_idx);
			md5_final(digest, &context);


			loop_idx = 0;
			buf[loop_idx] = 16;
			loop_idx++; // hash_len
			memcpy((uint8 *)(buf+loop_idx), (uint8 *)(digest), 16);
			loop_idx += 16; // hashed value
			memcpy((uint8 *)(buf+loop_idx), (uint8 *)(id), idlen);
			loop_idx += idlen; // id

			// read TX write pointer
			ptr = IINCHIP_READ(Sn_TX_WR0(0));
			ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ(Sn_TX_WR0(0) + 1);

		      	// copy data
			IINCHIP_WRITE_BUF(0, buf, (uint8*)ptr, loop_idx);

			// Update TX write pointer
			ptr += len;
			IINCHIP_WRITE(Sn_TX_WR0(0),(uint8)((ptr & 0xff00) >> 8));
			IINCHIP_WRITE((Sn_TX_WR0(0) + 1),(uint8)(ptr & 0x00ff));
		
			IINCHIP_WRITE(Sn_CR(0),Sn_CR_PCR);
	   }
   }
   else
   {
#ifdef __DEF_PPP_DBG__			
      printf("Not support PPP Auth type: %02bx%02bx\r\n",IINCHIP_READ(PATR0), IINCHIP_READ(PATR0+1));
#endif
      return PPP_RET_ERR_AUTHNA;
   }

   wait_10ms(100);
#ifdef __DEF_PPP_DBG__
   printf("Wait. Server's admission ");
#endif   
	loop_idx = 0;
	while (!((isr = getSn_IR(0)) & Sn_IR_PNEXT))
	{
#ifdef __DEF_PPP_DBG__
       printf("isr=%02bx ", isr);
#endif  	   
		if (isr & Sn_IR_PFAIL)
		{
			setSn_IR(0, 0xFF);	//added VER_3_7, all interrupt clear
#ifdef __DEF_IINCHIP_DBG__
         printf("failed\r\nReinput id, password..\r\n");
#endif     
			return PPP_RET_ERR_AUTHFAIL;
		}
#ifdef __DEF_IINCHIP_DBG__
      printf(".");
#endif      
		if (loop_idx++ == 10) // timeout
		{
			setSn_IR(0, 0xFF);	//added VER_3_7, all interrupt clear
#ifdef __DEF_IINCHIP_DBG__
         printf("timeout after PAP\r\n");
#endif         
			return PPP_RET_ERR_AUTH;
		}
		wait_10ms(100);
	}

	setSn_IR(0, 0xFF);	//added VER_3_7, all interrupt clear
	/* +200903[bj] clear receive buffer */
	len = getSn_RX_RSR(0);
	if ( len > 0 )
	{
		// read RX read pointer
		ptr = IINCHIP_READ(Sn_RX_RD0(0));
		ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ(Sn_RX_RD0(0) + 1);

		// copy data
		IINCHIP_READ_BUF(0,(uint8*)ptr,str, len);

		// Update RX read pointer
		ptr += len;
		IINCHIP_WRITE(Sn_RX_RD0(0),(uint8)((ptr & 0xff00) >> 8));
		IINCHIP_WRITE((Sn_RX_RD0(0) + 1),(uint8)(ptr & 0x00ff));
#ifdef __DEF_PPP_DBG__
		printf("dummy proc len = %u\r\n", len); for (i = 0; i < len; i++) printf ("%02bx ", str[i]); printf("\r\n");
#endif		
		IINCHIP_WRITE(Sn_CR(0),Sn_CR_RECV);
		while( IINCHIP_READ(Sn_CR(0)) );
	}
#ifdef __DEF_PPP_DBG__
   printf(" ok\r\nPHASE 4. IPCP\r\n");
#endif	

	// IP Address
	buf[0] = 0x03;
	buf[1] = 0x06;
	buf[2] = 0x00;
	buf[3] = 0x00;
	buf[4] = 0x00;
	buf[5] = 0x00;

	// read TX write pointer
	ptr = IINCHIP_READ(Sn_TX_WR0(0));
	ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ(Sn_TX_WR0(0) + 1);

   // copy data
	IINCHIP_WRITE_BUF(0, buf, (uint8*)ptr, 6);

	// Update TX write pointer
	ptr += 0x06;
	IINCHIP_WRITE(Sn_TX_WR0(0),(uint8)((ptr & 0xff00) >> 8));
	IINCHIP_WRITE((Sn_TX_WR0(0) + 1),(uint8)(ptr & 0x00ff));

	IINCHIP_WRITE(Sn_CR(0),Sn_CR_PCR);
	while( IINCHIP_READ(Sn_CR(0)) );
	wait_10ms(100);

	loop_idx = 0;
	while (1)
	{
		if (getSn_IR(0) & Sn_IR_PRECV)
		{
			setSn_IR(0, 0xFF);	//added VER_3_7, all interrupt clear
			len = getSn_RX_RSR(0);
			if ( len > 0 )
			{
				// read RX read pointer
				ptr = IINCHIP_READ(Sn_RX_RD0(0));
				ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ(Sn_RX_RD0(0) + 1);

				// copy data
				IINCHIP_READ_BUF(0,(uint8*)ptr,str, len);

				// Update RX read pointer
				ptr += len;
				IINCHIP_WRITE(Sn_RX_RD0(0),(uint8)((ptr & 0xff00) >> 8));
				IINCHIP_WRITE((Sn_RX_RD0(0) + 1),(uint8)(ptr & 0x00ff));

				IINCHIP_WRITE(Sn_CR(0),Sn_CR_RECV);
				while( IINCHIP_READ(Sn_CR(0)) );
				PutString("IPCP proc");  PutStringLn("");				
#ifdef __DEF_PPP_DBG__

				printf("IPCP proc\r\n"); for (i = 0; i < len; i++) printf ("%02bx ", str[i]); printf("\r\n");
#endif				
				str_idx = 6;
				dst_idx = 0;
				if (str[2] == 0x03) // in case of NAK
				{
					do
					{
						if (str[str_idx] == 0x03) // request only ip information
						{
							memcpy((uint8 *)(buf+dst_idx), (uint8 *)(str+str_idx), str[str_idx+1]);
							dst_idx += str[str_idx+1];
							str_idx += str[str_idx+1];
						}
						else
						{
							// skip byte
							str_idx += str[str_idx+1];
						}
						// for debug
						//printf("s: %u, d: %u, l: %u", str_idx, dst_idx, len);
					}
					while (str_idx != len);

					// read TX write pointer
					ptr = IINCHIP_READ(Sn_TX_WR0(0));
					ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ(Sn_TX_WR0(0) + 1);

				      	// copy data
					IINCHIP_WRITE_BUF(0, buf, (uint8*)ptr, dst_idx);

					// Update TX write pointer
					ptr += dst_idx;
					IINCHIP_WRITE(Sn_TX_WR0(0),(uint8)((ptr & 0xff00) >> 8));
					IINCHIP_WRITE((Sn_TX_WR0(0) + 1),(uint8)(ptr & 0x00ff));
					
					IINCHIP_WRITE(Sn_CR(0),Sn_CR_PCR); // send ipcp request
					while( IINCHIP_READ(Sn_CR(0)) );
					wait_10ms(100);
					break;
				}
			}
		}
#ifdef __DEF_IINCHIP_DBG__
      printf(".");
#endif      
		if (loop_idx++ == 10) // timeout
		{
#ifdef __DEF_IINCHIP_DBG__
         printf("timeout after IPCP\r\n");
#endif         
			return PPP_RET_ERR_IPCP;
		}
		wait_10ms(100);

		// read TX write pointer
		ptr = IINCHIP_READ(Sn_TX_WR0(0));
		ptr = ((ptr & 0x00ff) << 8) + IINCHIP_READ(Sn_TX_WR0(0) + 1);

	      	// copy data
		IINCHIP_WRITE_BUF(0, buf, (uint8*)ptr, 6);

		// Update TX write pointer
		ptr += 6;
		IINCHIP_WRITE(Sn_TX_WR0(0),(uint8)((ptr & 0xff00) >> 8));
		IINCHIP_WRITE((Sn_TX_WR0(0) + 1),(uint8)(ptr & 0x00ff));
		
		IINCHIP_WRITE(Sn_CR(0),Sn_CR_PCR); //ipcp re-request
		while( IINCHIP_READ(Sn_CR(0)) );
	}

	loop_idx = 0;
	while (!(getSn_IR(0) & Sn_IR_PNEXT))
	{
#ifdef __DEF_IINCHIP_DBG__
      printf(".");
#endif      
		if (loop_idx++ == 10) // timeout
		{
#ifdef __DEF_IINCHIP_DBG__
         printf("timeout after IPCP NAK\r\n");
#endif   
			return PPP_RET_ERR_IPCPNAK;
		}
		wait_10ms(100);
		IINCHIP_WRITE(Sn_CR(0),Sn_CR_PCR); // send ipcp request
		while( IINCHIP_READ(Sn_CR(0)) ); 
	}
	setSn_IR(0, 0xFF);	//added VER_3_7, all interrupt clear
#ifdef __DEF_PPP_DBG__
   printf(" ok\r\n");
#endif   
	return PPP_RET_SUCCESS;
	// after this function, User must save the pppoe server's mac address and pppoe session id in current connection
}


/******************************************************************************
@brief	Disconnect PPPoE connection
@input	mac: MAC address
				sessionid: Session ID of PPPoE connection
@output	1: disconnected the PPPoE connection
******************************************************************************/
uint8 pppterm(uint8 * mac, uint8 * sessionid)   large
{
	uint16 xdata i;
#ifdef __DEF_PPP_DBG__
	printf("pppterm()\r\n");
#endif
	/* Set PPPoE bit in MR(Common Mode Register) : enable socket0 pppoe */
	IINCHIP_WRITE(MR,IINCHIP_READ(MR) | MR_PPPOE);

	// write pppoe server's mac address and session id
	// must be setted these value.
	for (i = 0; i < 6; i++) IINCHIP_WRITE((Sn_DHAR0(0)+i),mac[i]);
	for (i = 0; i < 2; i++) IINCHIP_WRITE((Sn_DPORT0(0)+i),sessionid[i]);
	setSn_IR(0, getSn_IR(0));

	//open socket in pppoe mode
	IINCHIP_WRITE(Sn_MR(0),Sn_MR_PPPOE);
	IINCHIP_WRITE(Sn_CR(0),Sn_CR_OPEN);
	while( IINCHIP_READ(Sn_CR(0)) );
	wait_1us(1);
	
	// close pppoe connection
	IINCHIP_WRITE(Sn_CR(0),Sn_CR_PDISCON);
	while( IINCHIP_READ(Sn_CR(0)) );
	wait_10ms(100);
	// close socket
	IINCHIP_WRITE(Sn_CR(0),Sn_CR_CLOSE);
	while( IINCHIP_READ(Sn_CR(0)) );
	setSn_IR(0, 0xFF);	//added VER_3_7, all interrupt clear

#ifdef __DEF_PPP_DBG__
	printf("pppterm() end ..\r\n");
#endif
   IINCHIP_WRITE(MR,IINCHIP_READ(MR) & ~MR_PPPOE);
	return 1;
}

#endif

/******************************************************************************************
@brief	This function save the Subnet mask to Global variable 'SUBNET[4]' 
@input	addr
@output	Non
*****************************************************************************************/
void SetSubnet(uint8 * addr)
{
	SUBNET[0] = addr[0];
	SUBNET[1] = addr[1];
	SUBNET[2] = addr[2];
	SUBNET[3] = addr[3];
}

/******************************************************************************************
@brief	This function set the Subnet mask as user-entered Subnet mask  
@input	Non
@output	Non
*****************************************************************************************/
void ApplySubnet(void)
{
	/* Write SUBNETMASK of W7100 to SURR register */
	IINCHIP_WRITE (SUBR0+0, SUBNET[0]);
	IINCHIP_WRITE (SUBR0+1, SUBNET[1]);
	IINCHIP_WRITE (SUBR0+2, SUBNET[2]);
	IINCHIP_WRITE (SUBR0+3, SUBNET[3]);
}

/******************************************************************************************
@brief	This function claer the Subnet mask to '0.0.0.0' 
@input	Non
@output	Non
*****************************************************************************************/
void ClearSubnet(void)
{
	/* Write SUBNETMASK of W7100 to SURR register */
	IINCHIP_WRITE (SUBR0+0, 0);
	IINCHIP_WRITE (SUBR0+1, 0);
	IINCHIP_WRITE (SUBR0+2, 0);
	IINCHIP_WRITE (SUBR0+3, 0);
}

/******************************************************************************************
@brief	This function is used to get the subnet mask 
@input	Non
@output	Non
*****************************************************************************************/
void GetSubnet(uint8 * addr)
{
	/* Write SUBNETMASK of W7100 to SURR register */
	addr[0] = SUBNET[0];
	addr[1] = SUBNET[1];
	addr[2] = SUBNET[2];
	addr[3] = SUBNET[3];
}




