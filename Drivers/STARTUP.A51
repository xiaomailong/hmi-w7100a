$NOMOD51
;------------------------------------------------------------------------------
;  This file is part of the C51 Compiler package
;  Copyright (c) 1988-2005 Keil Elektronik GmbH and Keil Software, Inc.
;  Version 8.01
;
;  *** <<< Use Configuration Wizard in Context Menu >>> ***
;------------------------------------------------------------------------------
;  STARTUP.A51:  This code is executed after processor reset.
;
;  To translate this file use A51 with the following invocation:
;
;     A51 STARTUP.A51
;
;  To link the modified STARTUP.OBJ file to your application use the following
;  Lx51 invocation:
;
;     Lx51 your object file list, STARTUP.OBJ  controls
;
;------------------------------------------------------------------------------
;
;  User-defined <h> Power-On Initialization of Memory
;
;  With the following EQU statements the initialization of memory
;  at processor reset can be defined:
;
; <o> IDATALEN: IDATA memory size <0x0-0x100>
;     <i> Note: The absolute start-address of IDATA memory is always 0
;     <i>       The IDATA space overlaps physically the DATA and BIT areas.
IDATALEN        EQU     80H
;
; <o> XDATASTART: XDATA memory start address <0x0-0xFFFF> 
;     <i> The absolute start address of XDATA memory
XDATASTART      EQU     0     
;
; <o> XDATALEN: XDATA memory size <0x0-0xFFFF> 
;     <i> The length of XDATA memory in bytes.
XDATALEN        EQU     01000H      
;
; <o> PDATASTART: PDATA memory start address <0x0-0xFFFF> 
;     <i> The absolute start address of PDATA memory
PDATASTART      EQU     0H
;
; <o> PDATALEN: PDATA memory size <0x0-0xFF> 
;     <i> The length of PDATA memory in bytes.
PDATALEN        EQU     0H
;
;</h>
;------------------------------------------------------------------------------
;
;<h> Reentrant Stack Initialization
;
;  The following EQU statements define the stack pointer for reentrant
;  functions and initialized it:
;
; <h> Stack Space for reentrant functions in the SMALL model.
;  <q> IBPSTACK: Enable SMALL model reentrant stack
;     <i> Stack space for reentrant functions in the SMALL model.
IBPSTACK        EQU     0       ; set to 1 if small reentrant is used.
;  <o> IBPSTACKTOP: End address of SMALL model stack <0x0-0xFF>
;     <i> Set the top of the stack to the highest location.
IBPSTACKTOP     EQU     0xFF +1     ; default 0FFH+1  
; </h>
;
; <h> Stack Space for reentrant functions in the LARGE model.      
;  <q> XBPSTACK: Enable LARGE model reentrant stack
;     <i> Stack space for reentrant functions in the LARGE model.
XBPSTACK        EQU     0       ; set to 1 if large reentrant is used.
;  <o> XBPSTACKTOP: End address of LARGE model stack <0x0-0xFFFF>
;     <i> Set the top of the stack to the highest location.
XBPSTACKTOP     EQU     0xFFFF +1   ; default 0FFFFH+1 
; </h>
;
; <h> Stack Space for reentrant functions in the COMPACT model.    
;  <q> PBPSTACK: Enable COMPACT model reentrant stack
;     <i> Stack space for reentrant functions in the COMPACT model.
PBPSTACK        EQU     0       ; set to 1 if compact reentrant is used.
;
;   <o> PBPSTACKTOP: End address of COMPACT model stack <0x0-0xFFFF>
;     <i> Set the top of the stack to the highest location.
PBPSTACKTOP     EQU     0xFF +1     ; default 0FFH+1  
; </h>
;</h>
;------------------------------------------------------------------------------
;
;  Memory Page for Using the Compact Model with 64 KByte xdata RAM
;  <e>Compact Model Page Definition
;
;  <i>Define the XDATA page used for PDATA variables. 
;  <i>PPAGE must conform with the PPAGE set in the linker invocation.
;
; Enable pdata memory page initalization
PPAGEENABLE     EQU     0       ; set to 1 if pdata object are used.
;
; <o> PPAGE number <0x0-0xFF> 
; <i> uppermost 256-byte address of the page used for PDATA variables.
PPAGE           EQU     0
;
; <o> SFR address which supplies uppermost address byte <0x0-0xFF> 
; <i> most 8051 variants use P2 as uppermost address byte
PPAGE_SFR       DATA    0A0H
;
; </e>
;------------------------------------------------------------------------------


APP_START		EQU		01000H
; Standard SFR Symbols 
ACC     DATA    0E0H
B       DATA    0F0H
SP      DATA    81H
DPL     DATA    82H
DPH     DATA    83H

                NAME    ?C_STARTUP


?C_C51STARTUP   SEGMENT   CODE
?STACK          SEGMENT   IDATA

                RSEG    ?STACK
                DS      1

                EXTRN CODE (?C_START)
                PUBLIC  ?C_STARTUP

                CSEG    AT      0		  	
?C_STARTUP:     LJMP    STARTUP1			; Reset

				CSEG    AT      03H
				LJMP    APP_START+03H		; Int 0
				
				CSEG    AT      0BH
				LJMP    APP_START+0BH		; Int 1
				
				CSEG    AT      13H
				LJMP    APP_START+13H		; Int 2 
				
				CSEG    AT      1BH
				LJMP    APP_START+1BH	    ; Int 3
				
				CSEG    AT      23H
				LJMP    APP_START+23H 		; Int 4
				
				CSEG    AT      2BH
				LJMP    APP_START+2BH 		; Int 5
				
				CSEG	AT		33H
				LJMP	APP_START+33H		; Int 6

				CSEG	AT		3BH
				LJMP	APP_START+3BH		; Int 7

				CSEG    AT      43H
				LJMP    APP_START+43H 		; Int 8
				
				CSEG    AT      4BH
				LJMP    APP_START+4BH 		; Int 9

				CSEG    AT      53H
				LJMP    APP_START+53H 		; Int 10
				
				CSEG    AT      5BH
				LJMP    APP_START+5BH 		; Int 11
				
				CSEG    AT      63H
				LJMP    APP_START+63H 		; Int 12

				CSEG    AT      6BH
				LJMP    APP_START+6BH 		; Int 13

				CSEG    AT      73H
				LJMP    APP_START+73H 		; Int 14

				CSEG    AT      7BH
				LJMP    APP_START+7BH 		; Int 15

				CSEG    AT      83H
				LJMP    APP_START+83H 		; Int 16

				CSEG    AT      8BH
				LJMP    APP_START+8BH 		; Int 17

				CSEG    AT      93H
				LJMP    APP_START+93H 		; Int 18

				CSEG    AT      9BH
				LJMP    APP_START+9BH 		; Int 19

				CSEG    AT      0A3H
				LJMP    APP_START+0A3H 		; Int 20

				CSEG    AT      0ABH
				LJMP    APP_START+0ABH 		; Int 21

				CSEG    AT      0B3H
				LJMP    APP_START+0B3H 		; Int 22

				CSEG    AT      0BBH
				LJMP    APP_START+0BBH 		; Int 23

				CSEG    AT      0C3H
				LJMP    APP_START+0C3H 		; Int 24

				CSEG    AT      0CBH
				LJMP    APP_START+0CBH 		; Int 25

				CSEG    AT      0D3H
				LJMP    APP_START+0D3H 		; Int 26

				CSEG    AT      0DBH
				LJMP    APP_START+0DBH 		; Int 27

				CSEG    AT      0E3H
				LJMP    APP_START+0E3H 		; Int 28

				CSEG    AT      0EBH
				LJMP    APP_START+0EBH 		; Int 29

				CSEG    AT      0F3H
				LJMP    APP_START+0F3H 		; Int 30

				CSEG    AT      0FBH
				LJMP    APP_START+0FBH 		; Int 31




				RSEG    ?C_C51STARTUP

STARTUP1:
               ;---------------------------------------------------------------------
               ; Must be add the code to remove the 'APP_ENTRY' from code memory map.
               ; For detail, Refer to the datasheet of W7100.
               ;---------------------------------------------------------------------
               
               ANL		0FFH, #07FH

IF IDATALEN <> 0
                MOV     R0,#IDATALEN - 1
                CLR     A
IDATALOOP:      MOV     @R0,A
                DJNZ    R0,IDATALOOP
ENDIF

IF XDATALEN <> 0
                MOV     DPTR,#XDATASTART
                MOV     R7,#LOW (XDATALEN)
  IF (LOW (XDATALEN)) <> 0
                MOV     R6,#(HIGH (XDATALEN)) +1
  ELSE
                MOV     R6,#HIGH (XDATALEN)
  ENDIF
                CLR     A
XDATALOOP:      MOVX    @DPTR,A
                INC     DPTR
                DJNZ    R7,XDATALOOP
                DJNZ    R6,XDATALOOP
ENDIF

IF PPAGEENABLE <> 0
                MOV     PPAGE_SFR,#PPAGE
ENDIF

IF PDATALEN <> 0
                MOV     R0,#LOW (PDATASTART)
                MOV     R7,#LOW (PDATALEN)
                CLR     A
PDATALOOP:      MOVX    @R0,A
                INC     R0
                DJNZ    R7,PDATALOOP
ENDIF

IF IBPSTACK <> 0
EXTRN DATA (?C_IBP)

                MOV     ?C_IBP,#LOW IBPSTACKTOP
ENDIF

IF XBPSTACK <> 0
EXTRN DATA (?C_XBP)

                MOV     ?C_XBP,#HIGH XBPSTACKTOP
                MOV     ?C_XBP+1,#LOW XBPSTACKTOP
ENDIF

IF PBPSTACK <> 0
EXTRN DATA (?C_PBP)
                MOV     ?C_PBP,#LOW PBPSTACKTOP
ENDIF

                MOV     SP,#?STACK-1

; This code is required if you use L51_BANK.A51 with Banking Mode 4
;<h> Code Banking
; <q> Select Bank 0 for L51_BANK.A51 Mode 4
#if 0   
;     <i> Initialize bank mechanism to code bank 0 when using L51_BANK.A51 with Banking Mode 4.
EXTRN CODE (?B_SWITCH0)
                CALL    ?B_SWITCH0      ; init bank mechanism to code bank 0
#endif
;</h>
                LJMP    ?C_START

                END
