#ifndef DATAMSG_H
#define DATAMSG_H

enum{
	AM_DATAMSG=1,
};

typedef nx_struct datamsg{
	nx_uint8_t cmd[26];
	nx_uint16_t length;
}datamsg_t;

#endif