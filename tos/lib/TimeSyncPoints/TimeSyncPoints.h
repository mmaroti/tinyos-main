#ifndef TIMESYNCPOINTS_H
#define TIMESYNCPOINTS_H
 
 enum {
   AM_TIMESYNCPOINTS = 1,
   TIMESYNCPOINTS_PERIOD = 5000,
 };

typedef struct timeSyncPointsMsg{
	uint8_t nodeID;
}timeSyncPointsMsg_t; 

#endif