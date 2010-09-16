package org.szte.wsn.TimeSyncPoint;

public class LinearFunction {
	private double slope, offset;
	
	public double getSlope() {
		return slope;
	}

	public double getOffset() {
		return offset;
	}

	public LinearFunction(double offset, double slope){
		this.offset=offset;
		this.slope=slope;
	}
}
