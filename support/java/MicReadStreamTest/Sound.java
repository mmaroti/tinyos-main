/**
 * @(#)Sound.java
 *
 * Sound application
 *
 * @author 
 * @version 1.00 2009/11/5
 */

import static java.lang.System.out;
import java.io.*;
import javax.sound.sampled.*;


 
public class Sound {
	
	AudioFormat audioFormat;
	TargetDataLine targetDataLine;
	ByteArrayOutputStream byteArrayOutputStream;
	AudioInputStream audioInputStream;
	SourceDataLine sourceDataLine;
	
	public Sound(String filename,String amplification){
		captureAudio(filename,amplification);
		playAudio();
	}
       
    private void captureAudio(String filename,String amplification){
    	try{
      		//Get everything set up for capture
      		audioFormat = getAudioFormat();
      		DataLine.Info dataLineInfo = new DataLine.Info(TargetDataLine.class,audioFormat);
      		targetDataLine = (TargetDataLine)AudioSystem.getLine(dataLineInfo);
      		targetDataLine.open(audioFormat);
      		targetDataLine.start();
      		Capture(filename,amplification);
    	} catch (Exception e) {
      		System.err.println(e);
      		System.exit(0);
      	}
	}
	
	 private void playAudio() {
    	try{
	  	    //Get everything set up for playback.
	  	    //Get the previously-saved data into a byte array object.
      		byte audioData[] = byteArrayOutputStream.toByteArray();
      		//Get an input stream on the byte array containing the data
	 		InputStream byteArrayInputStream = new ByteArrayInputStream(audioData);
      		AudioFormat audioFormat = getAudioFormat();
      		audioInputStream =  new AudioInputStream(byteArrayInputStream,audioFormat,audioData.length/audioFormat.getFrameSize());
      		DataLine.Info dataLineInfo = new DataLine.Info(SourceDataLine.class,audioFormat);
      		sourceDataLine = (SourceDataLine)AudioSystem.getLine(dataLineInfo);
      		sourceDataLine.open(audioFormat);
      		sourceDataLine.start();
      		Play();
	   	} catch (Exception e) {
      			System.err.println(e);
      			System.exit(0);
   		}
  	}
	
   	private void Capture(String filename, String amplification){
  		byteArrayOutputStream = new ByteArrayOutputStream();
  		File file=new File(filename);
  		byte convBuffer[] = new byte[2];
        int convData=0;
        int amp=Integer.parseInt(amplification);
        int pos=0;
  		  		
  		try{
        	InputStream is= new FileInputStream(file);
        	long length = file.length();
        	byte tempBuffer[] = new byte[(int)length];
        	int readByteNum=is.read(tempBuffer);
        	//// Make the amplification and pack data to byteArrayOutputStream ///////////////////
        	while(pos<=readByteNum/2){
				convData=byteArrayToInt(tempBuffer,pos)*amp;
				if(convData <= 255){
        			convBuffer[0]=0;
        		}else{
        			convBuffer[0]=(byte)(convData>>8);
        		}
        		convBuffer[1]=(byte)convData;
        		byteArrayOutputStream.write(convBuffer, 0, 2);
        		pos+=2;
			}
    		byteArrayOutputStream.close();
			is.close();
    	} catch (IOException e) {
      		System.err.println(e);
      		System.exit(0);
    	} catch(IndexOutOfBoundsException e) {
    		e.printStackTrace(out);
    	}
  	}
  	 	
  	private void Play(){
  		byte tempBuffer[] = new byte[10000];
		try{
      		int cnt;
      		//Keep looping until the input read method returns -1 for empty stream.
      		while((cnt = audioInputStream.read(tempBuffer, 0,tempBuffer.length)) != -1){
        		if(cnt > 0){
	          		//Write data to the internal buffer of the data line
          			// where it will be delivered to the speaker.
          			sourceDataLine.write(tempBuffer, 0, cnt);
        		}
      		}
      		//Block and wait for internal buffer of the data line to empty.
      		sourceDataLine.drain();
      		sourceDataLine.close();
    	}catch (Exception e) {
      		System.out.println(e);
      		System.exit(0);
    	}
	}
	
	//This method creates and returns an AudioFormat object for a given set
    // of format parameters.  If these parameters don't work well for you, try some of the other
	// allowable parameter values, which are shown in comments following the declarations.
  	
  	private AudioFormat getAudioFormat(){
    	float sampleRate = 1800.0F;//8000,11025,16000,22050,44100
    	int sampleSizeInBits = 16;//8,16
    	int channels = 1;//1,2
    	boolean signed = true;//true,false
    	boolean bigEndian = true;//true,false
    	return new AudioFormat( sampleRate,sampleSizeInBits,channels,signed, bigEndian);
  	}
  	
  	public static int byteArrayToInt(byte[] b, int offset) {
        int value = 0;
        for (int i = 0; i < 2; i++) {
            int shift = (2 - 1 - i) * 8;
            value += (b[i + offset] & 0x000000FF) << shift;
        }
        return value;
    }
  	
  	public static void main(String[] args) {
    	
    	if(args.length<2)
    	{
    		System.err.println ("Usage: Sound [filename] [amplification(0-30)]");
    		System.exit(-1);
    	}
    	else if(Integer.parseInt(args[1])>30) {
    		System.err.println ("The amplification is to high!");
    		System.err.println ("It must be a value between from 0 to 30");
    		System.exit(-1);
    	}
    		
    	new Sound(args[0],args[1]);
    	System.out.println ("Ready!");
    }
  	
}
