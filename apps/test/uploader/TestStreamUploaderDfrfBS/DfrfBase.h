#ifndef DFRFBASE_H
#define DFRFBASE_H
#include "TimeSyncPoints.h"
enum {
  AM_TIME_MSG = AM_TIMESYNCPOINTS,
};

typedef struct time_msg{
    uint32_t remoteTime;
    uint32_t localTime;
    uint32_t sendTime;
}time_msg; 
#endif