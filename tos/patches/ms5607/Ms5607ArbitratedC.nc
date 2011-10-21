#include "Ms5607.h"
configuration Ms5607ArbitratedC
{
  provides interface Read<uint32_t> as ReadTemperature[uint8_t client]; 
  provides interface Read<uint32_t> as ReadPressure[uint8_t client];
  //You can't use the following interfaces if you're waiting for any readDone
  //the calibration data is always the same on the same chip, but this driver doesn't buffering it
  provides interface ReadRef<calibration> as ReadCalibration;
  provides interface Set<uint8_t> as SetPrecision;  
}
implementation
{
  components Ms5607C;
  ReadCalibration=Ms5607C;
  SetPrecision=Ms5607C;
  
  components new ArbitratedReadC(uint32_t) as ArbitratedTemp,
             new FcfsArbiterC(UQ_MS5607TEMP_RESOURCE) as TempArbiter,
             new Ms5607ClientP() as TempClient;
  
  ReadTemperature=ArbitratedTemp.Read;
  
  ArbitratedTemp.Resource->TempArbiter;
  ArbitratedTemp.Service->TempClient;
  
  TempClient.ActualRead->Ms5607C.ReadTemperature;
  
  
  components new ArbitratedReadC(uint32_t) as ArbitratedPress,
             new FcfsArbiterC(UQ_MS5607PRESS_RESOURCE) as PressArbiter,
             new Ms5607ClientP() as PressClient;
  
  ReadPressure=ArbitratedPress.Read;
  
  ArbitratedPress.Resource->PressArbiter;
  ArbitratedPress.Service->PressClient;
  
  PressClient.ActualRead->Ms5607C.ReadPressure;
}