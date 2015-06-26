/**********************************************
(c)COPYRIGHT WIZnet Inc. ALL RIGHT RESERVED
 W7100 driver
**********************************************/
/*
*
@file		wizmemcpy.h
@brief	Header file for wizmemcpy.c
*
*/

#ifndef __WIZMEMCPY_H
#define __WIZMEMCPY_H

#include "wizmemcpy.h"

#define ENTRY_FCOPY  0x07A2

void wizmemcpy(unsigned long fsrc, unsigned long fdst, unsigned int len) small;

#endif