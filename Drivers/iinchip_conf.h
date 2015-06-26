/**********************************************
(c)COPYRIGHT WIZnet Inc. ALL RIGHT RESERVED
 W7100 driver
**********************************************/
/*
*
@file		iinchip_conf.h
@brief	define address and value for Ethernet PHY control 
*
*/

#ifndef	_IINCHIP_CONF_H_
#define	_IINCHIP_CONF_H_

#define	MAX_SOCK_NUM		8	/**< Maxmium number of socket  */

/**
* __DEF_IINCHIP_xxx__ : define option for iinchip driver
*/
//#define __DEF_IINCHIP_DBG__ /* involve debug code in driver (socket.c) */

//#define __DEF_IINCHIP_INT__  /* involve interrupt service routine (TCPIPCore.c) if you want to use interrupt of W7100
							  							 /* You must!! set INTLEVEL0 register value higher than 0x3000 on NetInit()
							  								  Ex> IINCHIP_WRITE(INTLEVEL0, 0x50); IINCHIP_WRITE(INTLEVEL0 + 1, 0x00);*/
/**
* __DEF_IINCHIP_MAP_xxx__ : define memory map for iinchip
*/
#define COMMON_BASE 0x0000
#define __DEF_IINCHIP_MAP_TXBUF__ (COMMON_BASE + 0x8000) /* Internal Tx buffer address of the iinchip */
#define __DEF_IINCHIP_MAP_RXBUF__ (COMMON_BASE + 0xc000) /* Internal Rx buffer address of the iinchip */

#ifdef __DEF_IINCHIP_INT__
#define IINCHIP_ISR_DISABLE()	(EINT5 = 0)
#define IINCHIP_ISR_ENABLE()	(EINT5 = 1)
#else
#define IINCHIP_ISR_DISABLE()	
#define IINCHIP_ISR_ENABLE()	
#endif

#endif
