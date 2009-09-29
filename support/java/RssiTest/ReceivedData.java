
class ReceivedData {
	private short senderNodeID;
	private short receiverNodeID;
	private short rssiMin;
	private long rssiAver;
	private long rssiEnergy;
	private short rssiMax;
	private short lqiMin;
	private long lqiAver;
	private long lqiEnergy;
	private short lqiMax;
	private long sampleCnt;
	
			
	public short getsenderNodeID() {
		return (this.senderNodeID); 
	}

	public short getreceiverNodeID() {
		return (this.receiverNodeID); 
	}

	public short getrssiMin() {
		return (this.rssiMin); 
	}
	public long getrssiAver() {
		return (this.rssiAver); 
	}
	public long getrssiEnergy() {
		return (this.rssiEnergy); 
	}
	public long getlqiEnergy() {
		return (this.lqiEnergy); 
	}
	public short getrssiMax() {
		return (this.rssiMax); 
	}

	public short getlqiMin() {
		return (this.lqiMin); 
	}
	public long getlqiAver() {
		return (this.lqiAver); 
	}
	public short getlqiMax() {
		return (this.lqiMax); 
	}
	public long getsampleCnt() {
		return (this.sampleCnt); 
	}
	public void setsenderNodeID(short senderNodeID) {
		this.senderNodeID = senderNodeID; 
	}
	public void setreceiverNodeID(short receiverNodeID) {
		this.receiverNodeID = receiverNodeID; 
	}

	public void setrssiMin(short rssiMin) {
		this.rssiMin = rssiMin; 
	}
	public void setrssiAver(long rssiAver) {
		this.rssiAver = rssiAver; 
	}
	public void setrssiEnergy(long rssiEnergy) {
		this.rssiEnergy = rssiEnergy; 
	}
	public void setlqiEnergy(long lqiEnergy) {
		this.lqiEnergy = lqiEnergy; 
	}
	public void setrssiMax(short rssiMax) {
		this.rssiMax = rssiMax; 
	}
	public void setlqiMin(short lqiMin) {
		this.lqiMin = lqiMin; 
	}
	public void setlqiAver(long lqiAver) {
		this.lqiAver = lqiAver; 
	}
	public void setlqiMax(short lqiMax) {
		this.lqiMax = lqiMax; 
	}
	public void setsampleCnt(long sampleCnt) {
		this.sampleCnt = sampleCnt; 
	}
}
