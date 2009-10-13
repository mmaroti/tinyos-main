#ifndef PACKET_H
#define PACKET_H
 
 enum {
   AM_CONTROLPACKET = 1,
   AM_DATAPACKET=2,
 };

 typedef nx_struct controlpacket{
	nx_uint8_t nodeID;
	nx_uint8_t instr;
 }controlpacket_t;

 typedef nx_struct datapacket{
	nx_uint8_t senderNodeID;
	nx_uint8_t receiverNodeID;
	nx_uint32_t sampleCnt;
    nx_int8_t rssiMin;
	nx_int32_t rssiAver;
	nx_int32_t rssiEnergy;
	nx_int8_t rssiMax;
	nx_uint8_t lqiMin;
	nx_uint32_t lqiAver;
	nx_uint32_t lqiEnergy;
	nx_uint8_t lqiMax;
}datapacket_t;

#endif

