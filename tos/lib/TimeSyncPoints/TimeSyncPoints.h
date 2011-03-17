#ifndef TIMESYNCPOINTS_H
#define TIMESYNCPOINTS_H
 
enum {
  AM_TIMESYNCPOINTS = 1,
  TIMESYNCPOINTS_PERIOD = 600U,//10 minutes
};

typedef struct timeSyncPointsMsg{
    uint16_t bootCount;
    uint32_t timeStamp;
}timeSyncPointsMsg_t; 

#endif

