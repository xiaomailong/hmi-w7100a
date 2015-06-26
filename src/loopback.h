#ifndef _loopback_h__
#define _loopback_h__
 
#include "../Drivers/types.h"

void loopback_udp(SOCKET s, uint16 port, uint8 xdata * data_buf, uint16 mode);

#endif /*!_loopback_h__*/