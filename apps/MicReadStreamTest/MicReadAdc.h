#ifndef MICREADADC_H
#define MICREADADC_H

enum {
  AM_DATA_MSG = 1,
  AM_CTRL_MSG = 2,
  SAMPLE_PERIOD=1024000,
  MICROPHONE_WARMUP = 1200,
};


typedef nx_struct data_msg {
  nx_uint32_t sampleNum;
} data_msg_t;

typedef nx_struct ctrl_msg {
  nx_uint8_t instr, preScale;
} ctrl_msg_t;

#endif
