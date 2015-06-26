
#include <string.h>
#include "types.h"
#include "lcd.h"
#include "serial.h"

#define BUSY		0x80


void lcd_delay(uint16 xdata time) //(10ns*(time+1)*5)*4
{
  	vuint16 xdata i,j;
	while(time)
	{  
   		for(i=0;i<3;i++)
   			for(j=0;j<1;j++);
       			time--;
	 }
}



char lcd_ready(void)
{
   vuint16 xdata cnt=0;
  LCD_RWON();  
  while(1)  
    {
      P0_7 = 1;
      if(P0_7 == 0) break;
      if(cnt++ > 0xFFFE)
	     return 0;  
    }
  return 1;
}

void lcd_command(uint8 xdata Value)
{
  LCD_RSOFF();	
  LCD_RWOFF();
  
  LCD_DATA(Value);	
  
  lcd_delay(10);
  LCD_ENABLE();	
  lcd_delay(50);
  LCD_DISABLE();
  lcd_delay(10);
}


void lcd_clrscr(void)       
{
  lcd_command(0x01);
}


void lcd_datas(uint8 xdata ch)
{
  LCD_RSON();							//RS On
  LCD_RWOFF();	
  
  LCD_DATA(ch);
  lcd_delay(10);  
  LCD_ENABLE();	
  lcd_delay(50);
  LCD_DISABLE();
  lcd_delay(10);
}


char lcd_init(void)
{
  LCD_DISABLE();
  LCD_RSOFF();	
  LCD_RWOFF();
  
    lcd_delay(500);
    lcd_command(LCD_SET_SYSTEM | LCD_SYS_8BIT | LCD_SYS_2LINE);
  	lcd_delay(500);
	
	
	lcd_command(LCD_SET_DISPLAY | LCD_DISP_DISPLAY_ON);//| LCD_DISP_CURSOR_ON | LCD_DISP_CURSOR_FLASH);
	lcd_command(0x01);   
	lcd_command(LCD_CURSOR_HOME);
	  
 
  lcd_clrscr();
  lcd_gotoxy(0,0);

  return 1;
}


void lcd_gotoxy( uint8 x,   uint8 y )
{
  switch(y)
    {
    case 0 : lcd_command(0x80 + x); break;
    case 1 : lcd_command(0xC0 + x); break;
    case 2 : lcd_command(0x94 + x); break;
    case 3 : lcd_command(0xD4 + x); break;
    }
}



char * lcd_puts(uint8 * str)
{
	uint8 xdata i;
	for (i=0; str[i] != '\0'; i++){
	  lcd_delay(50);
	  lcd_datas(str[i]);
	}
	return str;
}



void lcd_putch(uint8 xdata ch)
{
  lcd_datas(ch);
}

void evb_set_lcd_text(uint8 row, uint8 * lcd)
{

  lcd_gotoxy(0,row);
  lcd_puts((char*)lcd);
}


