package org.szte.wsn.CSVProcess;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;

public class CSVMerger {
	//TODO get time/local from CSVHandler
	String separator;
	int globalColumn;
	ArrayList<CSVHandler> csvfiles;
	ArrayList<Integer> dataColumns=new ArrayList<Integer>();

	
	public CSVMerger(ArrayList<CSVHandler> inputFiles, ArrayList<Integer> dataColumns, int globalColumn, String separator) throws IOException{
		csvfiles=inputFiles;
		this.globalColumn=globalColumn;
		this.dataColumns=dataColumns;
		this.separator=separator;
	}
	
	public CSVMerger(ArrayList<CSVHandler> inputFiles, ArrayList<Integer> dataColumns, String globalName, String separator) throws IOException{
		csvfiles=inputFiles;
		globalColumn=csvfiles.get(0).getColumnNum(globalName);
		if(globalColumn<0)
			throw new NumberFormatException();
		this.dataColumns=dataColumns;
		this.separator=separator;
	}

	private ArrayList<String> createHeader(ArrayList<CSVHandler> csvfiles,	ArrayList<Integer> datacolumns, String nodeIdSeparator) {
		ArrayList<String> ret = new ArrayList<String>();
		ret.add(csvfiles.get(0).getHeaderId(globalColumn));
		for(int id:datacolumns){
			String stringid=csvfiles.get(0).getHeaderId(id);
			for(CSVHandler file:csvfiles){
				String filename=file.getName().substring(0, file.getName().lastIndexOf('.'));
				ret.add(stringid+nodeIdSeparator+filename);
			}
		}
		return ret;		
	}
	
	public CSVHandler createGlobalFile(File outfile, String nodeIdSepString, long startTime, long endTime) throws IOException {
		ArrayList<String> newHeader=createHeader(csvfiles, dataColumns, nodeIdSepString);
		ArrayList<Integer> newDC=new ArrayList<Integer>();
		for(int i=1;i<newHeader.size();i++)
			newDC.add(i+1);
		outfile.delete();
		CSVHandler globalFile=new CSVHandler(outfile, true, separator, 1, newDC);
		globalFile.setHeader(newHeader);

		long currenttime=startTime;
		int[] currentline= new int[csvfiles.size()];
		long[] currenttimes= new long[csvfiles.size()];
		for(int i=0;i<currentline.length;i++) currentline[i]=1;
		//get the next timestamp from all files
		for(int i=0;i<csvfiles.size();i++){
			for(;currentline[i]<=csvfiles.get(i).getLineNumber();currentline[i]++){
				currenttimes[i]=Long.parseLong(csvfiles.get(i).getCell(globalColumn, currentline[i]));
				if(currenttimes[i]>currenttime){
					break;
				}
			}
		}
		do{
			//search the minimum
			currenttime=Long.MAX_VALUE;
			int mintimefile=-1;
			for(int i=0;i<currenttimes.length;i++){
				if(currenttime>currenttimes[i]){
					currenttime=currenttimes[i];
					mintimefile=i;
				}
			}
			if(currenttime==Long.MAX_VALUE)
				break;//we run out of data
			//add the minimum to the file 
			ArrayList<String> newline=new ArrayList<String>();
			CSVHandler thisfile=csvfiles.get(mintimefile);
			newline.add(thisfile.getCell(globalColumn, currentline[mintimefile]));
			for(int column:dataColumns){
				for(int i=0;i<mintimefile;i++)
					newline.add("");
				newline.add(thisfile.getCell(column, currentline[mintimefile]));
				for(int i=mintimefile+1;i<csvfiles.size();i++)
					newline.add("");
			}
			globalFile.addLine(newline);
			//reread the used line
			String time=csvfiles.get(mintimefile).getCell(globalColumn, ++currentline[mintimefile]);
			if(time!=null)
				currenttimes[mintimefile]=Long.parseLong(time);
			else
				currenttimes[mintimefile]=Long.MAX_VALUE;
			
		}while(currenttime<endTime);		
		return globalFile;
	}


//	this should be in main
//	private void openInputFiles(ArrayList<File> inputFiles, boolean hasHeader) throws FileNotFoundException{
//		for(File inputFile:inputFiles){
//			try {
//				csvfiles.add(new CSVHandler(inputFile, hasHeader, separator));
//			} catch (IOException e) {
//				System.err.println("Can't read input file: "+inputFile.getName());
//			}
//		}
//		if(csvfiles.isEmpty())
//			throw new FileNotFoundException("No input files to merge");
//	}	

}
