#ifndef GSMSG_H
#define GSMSG_H

enum{
	AM_GSMMSG=1,
};

typedef nx_struct gsmmsg{
	nx_uint8_t 	cmd[10];
	nx_uint16_t length;
}gsmmsg_t;

#endif