Readme for Ucmini PlatformTest
Author/Contact: Zsolt Szabo, szabomeister@gmail.com

Description:

This is a test application for retrieving data from ucmini motes v1.0 and above. 
This mote contains a barometric pressure and temperature sensor (Intersema MS5607),
a humidity and temperature sensor (Sensirion SHT21),
and a light sensor (ROHM BH1750FVI). 
In this application, the SHT21 sensor is sampled first for temperature and then for humidity.
In the next step, the BH1750FVI sensor is sampled.
After this, the Intersema Ms5607 sensor is sampled, where the converted temperature and then humidity data is read.
The read values are sent to the basestation, and the sampling process starts again. 
The raw data read from the Sensirion Sht21 and BH1750FVI sensors have to be converted, according to the datasheets of the sensors.
The required conversions are done in the example java code.

By default this app can be used with ucminiv1.0.1-03 (all sensors mentioned above), no extra arguments are needed.
If intended to use with boards mounted with Sht21 sensor only, the SHT_ONLY command line argument must be specified.
e.g. : make CFLAGS+=-DSHT_ONLY ucmini 
 
The output normally looks like the following:

Sht21 Temp:   converted value (in degree centigrade
Sht21 Hum:    converted value (in %RH
Light:        converted value (in Lux or 0 if used with SHT_ONLY
Meas Temp:    mcu converted value (in degree centigrade or 0 if used with SHT_ONLY
Meas Press:   mcu converted value (in mbar or 0 if used with SHT_ONLY

Tools:
The PlatformTest.java is the example java code.

$Id: $


