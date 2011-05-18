#ifndef BMA180_H
#define BMA180_H

typedef struct bma180_data {
  uint16_t bma180_accel_x;
  uint16_t bma180_accel_y;
  uint16_t bma180_accel_z;
  uint8_t bma180_temperature;
} bma180_data_t;

#endif
