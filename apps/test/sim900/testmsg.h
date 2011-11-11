#ifndef TESTMSG_H
#define TESTMSG_H

enum{
	AM_TESTMSG=1,
};

typedef nx_struct testmsg{
	nx_uint8_t cmd[TOSH_DATA_LENGTH];
}testmsg_t;

#endif