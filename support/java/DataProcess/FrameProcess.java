
import java.io.BufferedReader;
import java.io.DataInputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FilenameFilter;
import java.io.InputStreamReader;

public class FrameProcess {
    private String[] fileNames;
    private FrameType[] frameTypes;
    public int typeNr;
    
	public String[] getFileNames() {
		return fileNames;
	}

	public void setFileNames(String[] fileNames) {
		this.fileNames = fileNames;
	}
	public FrameProcess(){
		frameTypes=new FrameType[5];
		typeNr=0;
	}
	public void setArgs(String[] args){
		File path = new File(args[0]);
		if (path.isFile()) {
			fileNames=new String[1];
			fileNames[0]=args[0];
		}
		if (path.isDirectory()){
			FilenameFilter filter = new FilenameFilter() 
			{ 
				public boolean accept(File path, String name) 
				{
					return name.endsWith("data.bin");
					}
				}; 
					
			fileNames = path.list(filter); 
			}
    		if (args.length>1) {
			try{		
			frameTypes=new FrameType[5];
			    frameTypes[0]=new FrameType();
			    FileInputStream fstream = new FileInputStream(args[1]);
			    DataInputStream in = new DataInputStream(fstream);
		        BufferedReader br = new BufferedReader(new InputStreamReader(in));
     		    String strLine;
    		    while ((strLine = br.readLine()) != null)   {
			     if(strLine.startsWith("typedef")){
    		    	 String[] reszek=strLine.split(" ");
			    	 if (reszek.length>2){
			     if (reszek[2].endsWith("{"))
			    	 reszek[2]=reszek[2].substring(0,reszek[2].length()-1); //eliminate "{" from the end of the type
		    	 frameTypes[0].setName(reszek[2]);
			    	 System.out.println(frameTypes[typeNr].getName());
			    	 }
			    	 else
			    		 System.out.println("Error: Wrong struct file1."); 
			     }
		    }
		  
     		    in.close();
			    }catch (Exception e){
			    	System.out.println("Error: Wrong struct file2."+ e.getMessage());
			    }
			
		}
		
		};

	
	public static void main(String[] args) {
		FrameProcess fP=new FrameProcess();
		fP.setArgs(args);
//		for (int i=0; i<fP.fileNames.length; i++)
//			System.out.print(fP.fileNames[i]);
		

	}
	
	
}
