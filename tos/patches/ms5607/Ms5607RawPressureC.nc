#include "Ms5607.h"
generic configuration Ms5607RawPressureC()
{
  provides interface Read<uint32_t>;
  //You can't use the following interfaces if you're waiting for any readDone
  //the calibration data is always the same on the same chip, but this driver doesn't buffering it
  provides interface ReadRef<calibration> as ReadCalibration;
  provides interface Set<uint8_t> as SetPrecision;  
}
implementation
{
  components Ms5607ArbitratedC, Ms5607C;
  ReadCalibration=Ms5607C;
  SetPrecision=Ms5607C;
  
  Read=Ms5607ArbitratedC.ReadPressure[unique(UQ_MS5607PRESS_RESOURCE)];
}