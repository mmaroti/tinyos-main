generic configuration SensirionSht21C() {
  provides interface Read<uint16_t> as Temperature;
  provides interface Read<uint16_t> as Humidity;
}
implementation {
  components SensirionSht21ReaderP();

  Temperature = SensirionSht21ReaderP.Temperature;
  Humidity    = SensirionSht21ReaderP.Humidity;

  components HalSensirionSht21C;
  
  enum { TEMP_KEY = unique("Sht21.Resource") };
  enum { HUM_KEY = unique("Sht21.Resource") };

  SensirionSht21ReaderP.TempResource -> HalSensirionSht21C.Resource[ TEMP_KEY ];
  SensirionSht21ReaderP.Sht11Temp -> HalSensirionSht21C.SensirionSht21[ TEMP_KEY ];
  SensirionSht21ReaderP.HumResource -> HalSensirionSht21C.Resource[ HUM_KEY ];
  SensirionSht21ReaderP.Sht11Hum -> HalSensirionSht21C.SensirionSht21[ HUM_KEY ];  
}
