package org.szte.wsn.dataprocess.parser;

public class Sht11TempParser extends IntegerParser{

	public Sht11TempParser(String name, String type) {
		super(name, type);
		signed=false;
		size=2;
	}
	
	@Override
	/**
	 * @return the temperature in celsius at the first place of the String array
	 */
	public String[] parse(byte[] packet)
	{	
		double ret=0.01*super.byteToLong(packet)-39.6;
		return new String[] {Double.toString(ret)};	
	}
	
	/**
	 * constructs byte[] from a temperature represented in String (in celsius)
	 * @param stringValue constructs byte[] from it 
	 * @return byte[]
	 */
	public byte[] construct(String[] stringValue)
	{
		double doubleValue=Double.parseDouble(stringValue[0]);
		long longValue=(long) ((doubleValue+39.6)*100);
		return super.longToByte(longValue);	
			
	}

}
