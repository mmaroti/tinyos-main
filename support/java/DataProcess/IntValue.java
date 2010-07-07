
public class IntValue implements Value{

	private int value;
	@Override
	public void getValue(byte[] bin) {	//generate int value from 2 big endian bytes	
		
	}
	public IntValue(byte[] bin){
		getValue(bin);
	}
	@Override
	public String toString() {
		// TODO Auto-generated method stub
		return ""+value;
	} 
	
}
