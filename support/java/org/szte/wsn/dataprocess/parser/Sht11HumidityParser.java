package org.szte.wsn.dataprocess.parser;

public class Sht11HumidityParser extends IntegerParser{

	public Sht11HumidityParser(String name, String type) {
		super(name, type);		
		signed=false;
		size=2;
	}
	
	@Override
	/**
	 * @return the humidity in percent at the first place of the String array
	 */
	public String[] parse(byte[] packet)
	{	
		long humi=super.byteToLong(packet);
		double ret=-4+0.0405*humi-2.8000E-6*humi*humi;
		return new String[] {Double.toString(ret)};	
	}
	
	/**
	 * constructs byte[] from a humidity represented in String (in percent)
	 * @param stringValue constructs byte[] from it 
	 * @return byte[]
	 */
	public byte[] construct(String[] stringValue)
	{
		double doubleValue=Double.parseDouble(stringValue[0]);
		doubleValue=(-0.0405+Math.sqrt(0.00159545-11.2E-6*doubleValue))/(-5.6E-6);
		long longValue=(long)doubleValue;
		return super.longToByte(longValue);	
			
	}

}

