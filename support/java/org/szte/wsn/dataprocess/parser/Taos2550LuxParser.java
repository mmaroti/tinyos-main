package org.szte.wsn.dataprocess.parser;

public class Taos2550LuxParser extends Taos2550Parser{

	public Taos2550LuxParser(String name, String type) {
		super(name, type);
		signed=false;
		size=2;
	}
	
	@Override
	/**
	 * @return the brightness (in Lux) at the first place of the String array
	 */
	public String[] parse(byte[] packet)
	{
		int visibleAndInfra=super.byteToCount(packet[0]);
		int infra=super.byteToCount(packet[1]);
		double lux;
		if(infra!=visibleAndInfra){
			double r=infra/(visibleAndInfra-infra);
			lux=(visibleAndInfra-infra)*0.39*Math.exp(-0.181*r*r);
		}else{
			lux=(visibleAndInfra-infra)/2;
		}
		
		return new String[]{Double.toString(lux)};
	}
	
	/**
	 * constructs byte[] from a brightness represented in String (in lux)
	 * @param stringValue constructs byte[] from it 
	 * @return byte[]
	 */
	public byte[] construct(String[] stringValue)
	{
		//TODO
		return null;	
	}

}
