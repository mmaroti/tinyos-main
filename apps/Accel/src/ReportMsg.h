#ifndef REPORT_MSG_H
#define REPORT_MSG_H

enum {
	AM_REPORTMSG = 11
};

typedef nx_struct ReportMsg {

	nx_uint16_t id;
	nx_uint8_t  mode;

} ReportMsg;

#endif /* REPORT_MSG_H */
