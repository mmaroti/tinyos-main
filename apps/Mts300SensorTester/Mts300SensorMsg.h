#ifndef SENSORMSG_H
#define SENSORMSG_H

enum{
	AM_CONTROLMSG = 1,
	AM_DATAMSG = 2,
};

typedef nx_struct controlmsg{
	nx_uint8_t nodeID;
	nx_uint8_t instr;
}controlmsg_t;

typedef nx_struct datamsg{
	nx_uint8_t nodeID;
	nx_uint16_t min;
	nx_uint16_t max;
	nx_uint16_t sampleCnt;
	nx_uint16_t micSampPer;
	nx_uint16_t micSampNum;
	nx_uint32_t average;
	nx_uint32_t energy;
}datamsg_t;

#endif


