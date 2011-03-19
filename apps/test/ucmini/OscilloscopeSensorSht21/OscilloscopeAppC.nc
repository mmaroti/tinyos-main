/*
 * Copyright (c) 2006 Intel Corporation
 * All rights reserved.
 *
 * This file is distributed under the terms in the attached INTEL-LICENSE     
 * file. If you do not find these files, copies can be found by writing to
 * Intel Research Berkeley, 2150 Shattuck Avenue, Suite 1300, Berkeley, CA, 
 * 94704.  Attention:  Intel License Inquiry.
 */

/**
 * Oscilloscope demo application. Uses the demo sensor - change the
 * new DemoSensorC() instantiation if you want something else.
 *
 * See README.txt file in this directory for usage instructions.
 *
 * @author David Gay
 */
configuration OscilloscopeAppC { }
implementation
{
  components OscilloscopeC, MainC, LedsC,
    new TimerMilliC(),
    Sht21C as Sensor, 
    //new Taos2550C() as Sensor,
    //Ms5607C as Sensor,
    //new AMSenderC(10) as Send,
    //ActiveMessageC as AMC;
		new SerialAMSenderC(10) as Send,
    SerialActiveMessageC as AMC;

  OscilloscopeC.Boot -> MainC;
  OscilloscopeC.AMSend -> Send;
  OscilloscopeC.AMControl -> AMC;
  OscilloscopeC.Read -> Sensor.Temperature;
  //OscilloscopeC.Read -> Sensor.VisibleLight;
  OscilloscopeC.Leds -> LedsC;

  OscilloscopeC.I2CControl -> Sensor.SplitControl; 
}
