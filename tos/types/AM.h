#ifndef AM_H
#define AM_H

typedef nx_uint8_t nx_am_id_t;
typedef uint8_t am_id_t;

typedef nx_uint8_t nx_am_group_t;
typedef uint8_t am_group_t;

#ifdef RFXLINK_64BIT_ADDR
typedef nx_uint64_t nx_am_addr_t;
typedef uint64_t am_addr_t;
#else
typedef nx_uint16_t nx_am_addr_t;
typedef uint16_t am_addr_t;
#endif

enum {
  AM_BROADCAST_ADDR = 0xffff,
};

#ifndef DEFINED_TOS_AM_GROUP
#define DEFINED_TOS_AM_GROUP 0x22
#endif

#ifndef DEFINED_TOS_AM_ADDRESS
#define DEFINED_TOS_AM_ADDRESS 1
#endif

enum {
  TOS_AM_GROUP = DEFINED_TOS_AM_GROUP,
  TOS_AM_ADDRESS = DEFINED_TOS_AM_ADDRESS
};

#define UQ_AMQUEUE_SEND "amqueue.send"

#endif

