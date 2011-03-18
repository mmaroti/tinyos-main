package org.szte.wsn.dataprocess.parser;

public class Taos2550Parser extends IntegerParser{

	public Taos2550Parser(String name, String type) {
		super(name, type);		
		signed=false;
		size=1;
	}
	
	/**
	 * @return the count value of the light sensor at the first place of the String array
	 */
	protected int byteToCount(byte packet)
	{	
		int count=(int) super.byteToLong(new byte[] {packet});
		count&=0x7f;
		int s=0xf&count;
		int c=Integer.rotateRight(count&0x70,4);
		int twopowc=(int) Math.pow(2,c);
		count=(int) (Math.floor(16.5*(twopowc-1)))+s*twopowc;

		return count;	
	}
	
	@Override
	/**
	 * @return the brightness (in COUNT: see the datasheet) at the first place of the String array
	 */
	public String[] parse(byte[] packet)
	{	
		int light=byteToCount(packet[0]);

		return new String[] {Integer.toString(light)};	
	}
	
	/**
	 * constructs byte[] from a humidity represented in String (in percent)
	 * @param stringValue constructs byte[] from it 
	 * @return byte[]
	 */
	public byte[] construct(String[] stringValue)
	{
		//TODO
		return null;	
	}

}

