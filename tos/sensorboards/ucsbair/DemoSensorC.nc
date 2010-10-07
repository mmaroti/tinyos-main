generic configuration DemoSensorC() {
  provides interface Read<uint16_t>;
}
implementation {
  components new CarbonC() as Sensor;
  Read = Sensor;
}
