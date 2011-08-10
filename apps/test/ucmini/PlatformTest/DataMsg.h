#ifndef DATAMSG_H
#define DATAMSG_H

enum{
	AM_DATAMSG=1,
};

typedef nx_struct datamsg{
	nx_uint16_t ShtTemp_data;
	nx_uint16_t ShtHum_data;
	nx_int16_t MsTemp_data;
	nx_uint32_t MsPress_data;
	nx_uint16_t Light_data;
}datamsg_t;

#endif
