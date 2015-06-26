#ifndef __SERIAL_H
#define __SERIAL_H

/*
###############################################################################
Include Part
###############################################################################
*/
//#include <TYPE.H>


/*
###############################################################################
Define Part
###############################################################################
*/
#define X2MODE				// For MCU supported X2 Mode

/*
If there are unused functions in program and on compiling with Keil-C, it result in warning.In this reason, wrong operation could be happend.
So it prevent to compile unused functions to use define-function.
*/
#define __SERIAL_USED   // If defined with "  __SERIAL_UNUSED", actually it's not to be compiled " __SERIAL_UNUSED Block"

/* 
[ Notice ]
if UART Application sample code, "APP_UART" and one of "UART_MODEn_TIMERn" must be enabled. 
*/
//#define APP_UART
#ifdef APP_UART 

#define UART_MODE1_TIMER1
//#define UART_MODE1_TIMER2
//#define UART_MODE3_TIMER1
//#define UART_MODE3_TIMER2

#endif


/*
###############################################################################
Grobal Variable Definition Part
###############################################################################
*/



/*
###############################################################################
Function Prototype Definition Part
###############################################################################
*/

void InitSerial(void);				/* Initialization of Serial Port(Ex.Baud Rate setting) */	
void InitSerial_Interrupt(void);	/* Initialization of Serial Port - Serial interrupt enable */


/*
###############################################################################
Unused Function Prototype Definition Part
###############################################################################
*/

#ifndef __SERIAL_UNUSED

void GetString(char* str);		/* Read 1 line string from Serial. */
char putchar (char c);

#endif  //  end __SERIAL_UNUSED


#endif  //  end __SERIAL_H
