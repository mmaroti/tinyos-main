
public class LongValue implements Value{

	private long value;
	public LongValue(byte[] bin){
		getValue(bin);
	}
	@Override
	public void getValue(byte[] bin) {
		
		// TODO Auto-generated method stub		
	}
	@Override
	public String toString() {
		// TODO Auto-generated method stub
		return ""+value;
	}

}
