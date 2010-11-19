#ifndef SERIAL_ADDER_H
#define SERIAL_ADDER_H

typedef nx_struct test_serial_msg {
  nx_uint16_t first;
  nx_uint16_t second;
  nx_uint16_t result;
} test_serial_msg_t;

enum {
  AM_TEST_SERIAL_MSG = 0x89,
};

#endif
