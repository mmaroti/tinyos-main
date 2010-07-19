
public class SampleParser extends PacketParser {

	byte first;
	
	public SampleParser(String name, byte first)
	{
		this.name = name;
		this.first = first;
	}
	
	@Override
	public String[] parse(byte[] packet) {
		if( packet.length != 2 || packet[0] != first )
			return null;
		
		return new String[] { Byte.toString(packet[1]) };
	}

	@Override
	public int getPacketLength() {
		return 2;
	}

	@Override
	public String[] getFields() {
		return new String[] { "second" };
	}

}
