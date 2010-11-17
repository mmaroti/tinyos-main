#ifndef SYNC_MSG_H
#define SYNC_MSG_H

enum {
	AM_SYNCMSG = 0x44
};

typedef nx_struct {

	nx_uint32_t first_block;
	nx_uint32_t event_time;

} SyncMsg;

enum {

	PAYLOAD_LENGTH = sizeof(SyncMsg)
};

#endif /* SYNC_MSG_H */
