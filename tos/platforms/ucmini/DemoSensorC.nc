/**
  *
  * @author Zsolt Szabó <szabomeister@gmail.com>
  */

generic configuration DemoSensorC() {
  provides interface Read<uint16_t>;
}
implementation {
  //components new Atm128InternalTemperatureC() as Sensor;
  //Read = Sensor;
  components Sht21C as Sensor;
  Read = Sensor.Temperature;
  //components bh1750fviC as Sensor;
  //Read = Sensor.Light;
}
