generic configuration TemperatureC() {
  provides interface Read<uint16_t>;
}
implementation {
  components new SensirionSht11C();
  Read = SensirionSht11C.Temperature;
}
