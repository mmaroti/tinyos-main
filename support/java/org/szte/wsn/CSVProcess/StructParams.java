package org.szte.wsn.CSVProcess;

import java.io.File;
import java.util.ArrayList;


public class StructParams{
	private int localColumn; //time *
	private int globalColumn; //time *
	private ArrayList<Integer> dataColumns;  // *
	private File outputFile; //*
	private File avgOutputFile;	
	private boolean insertGlobal;	
	private String name;

	public StructParams(int localColumn, int globalColumn, int[] dataColumns, String outputName, String avgName, boolean insertGlobal, String name){
		this.localColumn=localColumn;
		this.globalColumn=globalColumn;
		this.dataColumns=new ArrayList<Integer>();
		for(int i=0;i<dataColumns.length;i++)
			this.dataColumns.add(dataColumns[i]);
		this.outputFile = new File(outputName);
		this.avgOutputFile = new File(avgName);
		this.insertGlobal=insertGlobal;
		this.name=name;
	}

	public File getOutputFile() {
		return outputFile;
	}
	public boolean isInsertGlobal() {
		return insertGlobal;
	}
	public File getAvgOutputFile() {
		return avgOutputFile;
	}
	public int getGlobalColumn() {
		return globalColumn;
	}
	public int getLocalColumn() {
		return localColumn;
	}
	public String getName(){
		return name;
	}

	public ArrayList<Integer> getDataColumns() {
		return dataColumns;
	}
}
