#ifndef GREEN_HOUSE_H
#define GREEN_HOUSE_H

#include <AM.h>
#include <message.h>

enum
{
	CL_TEST = 0xee,
	/* Number of readings per message. If you increase this, you may have to
	increase the message_t size. */
	NREADINGS = 4,
	//NSERIALMSGS = 10,
	QUEUE_SIZE = 10,
	/* Default sampling period. */
	DEFAULT_INTERVAL = 1000,

	AM_GREENHOUSE = 0x77
};

typedef nx_struct greenhouse {
  nx_am_addr_t source;
  nx_uint16_t seqno;
  nx_am_addr_t parent;
  nx_uint16_t metric;
  nx_uint16_t data[NREADINGS];
//  nx_uint8_t hopcount;
//  nx_uint16_t sendCount;
//  nx_uint16_t sendSuccessCount;
} GH_Msg;
/*
typedef nx_struct greenhouse {
  GH_Msg messages[NSERIALMSGS];
} UART_Msg;
*/
#endif /* GREEN_HOUSE_H */
