#ifndef TIMESYNCPOINTS_H
#define TIMESYNCPOINTS_H
 
enum {
  AM_TIMESYNCPOINTS = 1,
  AM_TIMESYNCREF = AM_TIMESYNCPOINTS+1,
  TIMESYNCPOINTS_PERIOD = 600U,//10 minutes
  REFERENCE_NODEID = 0,
};

typedef struct timeSyncPointsMsg{
    uint32_t timeStamp;
}timeSyncPointsMsg_t; 

typedef struct referenceMsg{
    uint32_t nodeTime;
    uint32_t refTime;
    am_addr_t nodeid;
}referenceMsg_t; 

#endif

