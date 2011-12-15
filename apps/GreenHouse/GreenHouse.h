#ifndef GREEN_HOUSE_H
#define GREEN_HOUSE_H

#include <AM.h>
#include <message.h>

enum{
	
	CL_TEST = 0xee,
	MEASURINGS = 10
};

typedef nx_struct GreenHouseMSG {
  nx_am_addr_t source;
  nx_uint16_t seqno;
  nx_am_addr_t parent;
  nx_uint16_t metric;
  nx_uint16_t data[MEASURINGS];
//  nx_uint8_t hopcount;
  nx_uint16_t sendCount;
  nx_uint16_t sendSuccessCount;
} GH_Msg;

#endif /* GREEN_HOUSE_H */
