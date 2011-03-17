#ifndef DFRFBASE_H
#define DFRFBASE_H
//#include "TimeSyncPoints.h"
enum {
  AM_TIME_MSG = 1, //==AM_TIMESYNCPOINTS,
};

typedef struct time_msg{
    nx_uint32_t remoteTime;
    nx_uint32_t localTime;
    nx_uint32_t sendTime;
    nx_uint16_t bootCount;
}time_msg; 
#endif