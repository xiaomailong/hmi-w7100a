
#include "W7100.h"

char _getkey ()  reentrant
{
 	unsigned char byData;
	// Wait till data is received.
	while(!RI);		
	RI = 0;
	// Read data.
	byData = SBUF;		
	return byData;
}
