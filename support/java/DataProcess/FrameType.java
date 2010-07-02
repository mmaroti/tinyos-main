public class FrameType {
		Variable[] variables ;
		byte size;
		String name;
		public String getName() {
			return name;
		}
		public void setName(String name) {
			this.name = name;
		}
		public FrameType(){
			variables=new Variable[10];
		}
		public Variable[] getVariables() {
			return variables;
		}
		public void setVariables(Variable[] variables) {
			this.variables = variables;
		}
		public byte getSize() {
			return size;
		}
		public void setSize(byte size) {
			this.size = size;
		}		
		}
