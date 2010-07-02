public class Variable{
		String name;
		byte size;		
		boolean nxle;
		boolean signed;
		long value;
		
		public Variable(){
			
		}
		public Variable(String name, byte size, boolean nxle, boolean signed, long value){
			
		}
		
		
		public String getName() {
			return name;
		}
		public void setName(String name) {
			this.name = name;
		}
		public byte getSize() {
			return size;
		}
		public void setSize(byte size) {
			this.size = size;
		}
		public boolean isNxle() {
			return nxle;
		}
		public void setNxle(boolean nxle) {
			this.nxle = nxle;
		}
		public boolean isSigned() {
			return signed;
		}
		public void setSigned(boolean signed) {
			this.signed = signed;
		}
		
	}
