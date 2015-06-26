#include "W7100.h"

char putchar (char c)  
{
 	// Write data into serial-buffer.
	SBUF = c; 
	// Wait till data recording is finished.
	while(!TI);
	TI = 0;
	return c;
}


