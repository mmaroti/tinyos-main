#ifndef GREEN_HOUSE_H
#define GREEN_HOUSE_H

#include <AM.h>
#include <message.h>

enum
{
	CL_TEST = 0xee,
	/*Mérések száma elküldött csomagonként.*/
	/*Növelése esetén esetleg meg kell növelni a message_t méretét (alapértelmezetten: 29 bájt*/
	NREADINGS = 2,
	/*Küldési sorok mérete.*/
	QUEUE_SIZE = 10,
	/*Szenzorok mintavételezési periódusa*/
	DEFAULT_INTERVAL = 1000,
	/*Aktív üzenet azonosítója a csomagtípushoz.*/
	AM_GREENHOUSE = 0x77
};

/*A hálózaton továbbított üzenettípus.*/
typedef nx_struct greenhouse {
  nx_am_addr_t source;
  nx_uint16_t seqno;
  nx_am_addr_t parent;
  nx_uint16_t metric;
  nx_uint16_t data[NREADINGS];
} GHMsg;

#endif /* GREEN_HOUSE_H */
