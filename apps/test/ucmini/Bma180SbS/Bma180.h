#ifndef BMA180_H
#define BMA180_H

typedef struct bma180_data {
  uint16_t bma180_accel_x;
  uint16_t bma180_accel_y;
  uint16_t bma180_accel_z;
  uint8_t bma180_temperature;
  uint8_t chip_id;
} bma180_data_t;

enum {
  BMA_SAMPLING_TIME_MS = 64,
};

/* 0: low noise, higest current, full bandwidth(1200Hz)
   1: super low noise, highest current, reduced bandwidth (300Hz)
   2: ultra low noise, smaller current, reduced bandwidth (150Hz)
   3: Low power mode, lowest current, higher noise than other modes
*/
enum {
  BMA_MODE = 2,  
};

/*
    0: 1g   1: 1.5g   2: 2g
    3: 3g   4: 4g     5: 8g
    6: 16g  7: NA
*/
enum {
  BMA_RANGE = 2,
};
#endif
