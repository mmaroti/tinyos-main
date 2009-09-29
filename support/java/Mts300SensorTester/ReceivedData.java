
class ReceivedData {
	private short nodeID;
	private int min;
	private int max;
	private long average;
	private long energy;
	private int sampleCnt;
	private int micSampPer;
	private int micSampNum;
	
	public short getNodeID() {
		return (this.nodeID); 
	}

	public int getMin() {
		return (this.min); 
	}

	public int getMax() {
		return (this.max); 
	}

	public long getAverage() {
		return (this.average); 
	}

	public long getEnergy() {
		return (this.energy); 
	}
	
	public int getsampleCnt() {
		return (this.sampleCnt); 
	}
	public int getmicSampPer() {
		return (this.micSampPer); 
	}
	public int getmicSampNum() {
		return (this.micSampNum); 
	}
		
	public void setNodeID(short nodeID) {
		this.nodeID = nodeID; 
	}

	public void setMin(int min) {
		this.min = min; 
	}

	public void setMax(int max) {
		this.max = max; 
	}

	public void setAverage(long average) {
		this.average = average; 
	}

	public void setEnergy(long energy) {
		this.energy = energy; 
	}
	public void setsampleCnt(int sampleCnt) {
		this.sampleCnt = sampleCnt; 
	}
	public void setmicSampPer(int micSampPer) {
		this.micSampPer = micSampPer; 
	}
	public void setmicSampNum(int micSampNum) {
		this.micSampNum = micSampNum; 
	}
}
