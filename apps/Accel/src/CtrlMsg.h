#ifndef CTRL_MSG_H
#define CTRL_MSG_H

enum {
	AM_CTRLMSG = 9
};

typedef nx_struct CtrlMsg {

	nx_uint8_t cmd;

} CtrlMsg;

#endif /* CTRL_MSG_H */
