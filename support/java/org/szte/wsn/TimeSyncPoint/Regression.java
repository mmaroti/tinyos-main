package org.szte.wsn.TimeSyncPoint;

import java.util.ArrayList;

public class Regression {
	private ArrayList<Point> points=new ArrayList<Point>();
	private boolean functionNeedsUpdate=true;
	private LinearFunction function;
	private long maxerror;
	private double defaultslope;

	private final class Point{
		private long x,y;
		public long getX() {
			return x;
		}
		public long getY() {
			return y;
		}
		
		public Point(long x, long y){
			this.x=x;
			this.y=y;
		}
	}
	
	public Regression(long maxerror, double defaultslope){
		this.maxerror=maxerror;
		this.defaultslope=defaultslope;
	}	
	
	public long getMaxerror() {
		return maxerror;
	}

	public void setMaxerror(long maxerror, double defaultslope) {
		this.maxerror = maxerror;
		this.defaultslope = defaultslope;
	}
	
	public long getNumPoints() {
		return points.size();
	}
	
	private double calcError(Point pt){
		if(points.size()==0)
			return 0;
		else {
			if(functionNeedsUpdate){
				calculateFunction();
			}
			double ret=pt.getY()-function.getOffset()-function.getSlope()*pt.getX();
			if(ret<0)
				return -1*ret;
			else	
				return ret;
		}
	}
	
	public boolean addPoint(long x,long y){
		Point p=new Point(x,y);
		if(calcError(p)<maxerror){
			points.add(new Point(x,y));
			functionNeedsUpdate=true;
			return true;
		}else
			return false;
	}
	


	public void addPointNoVerify(long x,long y){
		points.add(new Point(x,y));
		functionNeedsUpdate=true;
	}
	
	public LinearFunction getFunction(){
		if(functionNeedsUpdate){
			calculateFunction();
		}
		return function;
	}

	private LinearFunction calculateFunction() {
		double slope,offset;
		if(points.size()<=0)
			return null;
		if(points.size()>=2){//linear regression
			double avg_x=0, avg_y=0;
			for(Point pt:points){
				avg_x+=pt.getX();
				avg_y+=pt.getY();
			}
			avg_x/=points.size();
			avg_y/=points.size();
			double denom=0,numer=0;
			for(Point pt:points){
				numer+=(pt.getX()-avg_x)*(pt.getY()-avg_y);
				denom+=(pt.getX()-avg_x)*(pt.getX()-avg_x);
			}
			slope=numer/denom;
			offset=avg_y-slope*avg_x;							
		} else{//only one point => we suppose that slope==defaultslope
			slope=defaultslope;
			offset=points.get(0).getY()-points.get(0).getX();
		}
		function=new LinearFunction(offset, slope);
		functionNeedsUpdate=false;
		return function;
	}

	public void setDefaultslope(double defaultslope) {
		this.defaultslope = defaultslope;
	}

	public double getDefaultslope() {
		return defaultslope;
	}
	
	
}
