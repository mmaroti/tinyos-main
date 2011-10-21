import java.io.IOException;

import net.tinyos.message.*;
import net.tinyos.packet.*;
import net.tinyos.util.*;

public class UcminiSensor implements MessageListener {

  private MoteIF moteIF;
  private int calibration[]=new int[6];
  public UcminiSensor(MoteIF moteIF) {
    this.moteIF = moteIF;
    this.moteIF.registerListener(new UcminiSensorCalib(), this);
    this.moteIF.registerListener(new UcminiSensorMeas(), this);
  }

  public void messageReceived(int to, Message message) {
    if(message instanceof UcminiSensorCalib){
    	UcminiSensorCalib msg = (UcminiSensorCalib)message;
    	calibration[0]=msg.getElement_coefficient(0);
    	calibration[1]=msg.getElement_coefficient(1);
    	calibration[2]=msg.getElement_coefficient(2);
    	calibration[3]=msg.getElement_coefficient(3);
    	calibration[4]=msg.getElement_coefficient(4);
    	calibration[5]=msg.getElement_coefficient(5);
    	System.out.println("Ms5607 calibration:");
    	System.out.println("c1="+calibration[0]+"; c2="+calibration[1]+"; c3="+calibration[2]+"; c4="+calibration[3]+
    					   "; c5="+calibration[4]+"; c6="+calibration[5]);
    } else if(message instanceof UcminiSensorMeas){
      UcminiSensorMeas msg = (UcminiSensorMeas)message;
      System.out.println("Humidity (sht21):\t\t"+msg.get_humi());
      System.out.println("Temperature (sht21):\t\t"+msg.get_temp());
      System.out.println("Temperature (atmega128rfa1):\t"+msg.get_temp3());
      System.out.println("Temperature (ms5607):\t\t"+msg.get_temp2());
      System.out.println("Pressure (ms5607):\t\t"+msg.get_press());
      System.out.println("Light (bh1750fvi):\t\t"+msg.get_light());
      System.out.println("Voltage (atmega128rfa1):\t"+msg.get_voltage());
      System.out.println();
    }
  }
  
  private static void usage() {
    System.err.println("usage: UcminiSensor [-comm <source>]. Default source: serial@/dev/ttyUSB0:ucmini");
  }
  
  public static void main(String[] args) throws Exception {
    String source = null;
    if (args.length == 2) {
      if (!args[0].equals("-comm")) {
        usage();
        System.exit(1);
      }
      source = args[1];
    }
    else if (args.length != 0) {
      usage();
      System.exit(1);
    }
    
    PhoenixSource phoenix;
    if (source == null) {
      source = "serial@/dev/ttyUSB0:ucmini";
    }
    phoenix = BuildSource.makePhoenix(source, PrintStreamMessenger.err);

    MoteIF mif = new MoteIF(phoenix);
    UcminiSensor serial = new UcminiSensor(mif);
  }


}
