/** Copyright (c) 2009, University of Szeged
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* - Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
* - Redistributions in binary form must reproduce the above
* copyright notice, this list of conditions and the following
* disclaimer in the documentation and/or other materials provided
* with the distribution.
* - Neither the name of University of Szeged nor the names of its
* contributors may be used to endorse or promote products derived
* from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
* SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
* STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
* OF THE POSSIBILITY OF SUCH DAMAGE.
*
* Author: Zoltan Kincses
*/

import static java.lang.System.out; 
import javax.swing.*;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import java.awt.*;
import java.awt.event.*;
import java.io.*;
import javax.sound.sampled.*;
import net.tinyos.message.*;
import net.tinyos.util.*;
import net.tinyos.packet.*;

class Capture extends JFrame implements MessageListener {

	PhoenixSource phoenix;
	MoteIF mif;
	
	boolean captureAudio = false;
	boolean firstTime = true;
	long missedPktNum,bufferNum,bufferSize,receivedPktNum;
//	int gain;
	
	File wavFile;
	RandomAccessFile wavFileHandler;
	TextField outputText;
			
	public static void main(String args[]){
		new Capture();
	}

	public Capture(){
	
		final JButton captureBtn = new JButton("Capture");
		final JButton stopBtn = new JButton("Stop");
		final JButton playBtn = new JButton("Playback");
		
//		final JSlider slider = new JSlider(JSlider.VERTICAL, 0, 2, 0);
		
		outputText = new TextField();
			
		phoenix=BuildSource.makePhoenix("serial@com24:460800", PrintStreamMessenger.err);
	    mif = new MoteIF(phoenix);
	    mif.registerListener(new MicMsg(),this);
    
	    captureBtn.setEnabled(true);
		stopBtn.setEnabled(false);
		playBtn.setEnabled(false);

		//Register anonymous listeners
		captureBtn.addActionListener(new ActionListener(){
			public void actionPerformed(ActionEvent e){
				captureBtn.setEnabled(false);
				stopBtn.setEnabled(true);
				playBtn.setEnabled(false);
				//Capture input data from the mote until the Stop button is clicked.
				initCapture();
				}
			}	
		);
		getContentPane().add(captureBtn);

		stopBtn.addActionListener(new ActionListener(){
			public void actionPerformed(ActionEvent e){
				captureBtn.setEnabled(true);
				stopBtn.setEnabled(false);
				playBtn.setEnabled(true);
				//Terminate the capturing of input data from the mote.
				endCapture();
				}
			}
		);
		getContentPane().add(stopBtn);

		playBtn.addActionListener(new ActionListener(){
			public void actionPerformed(ActionEvent e){
				//Play back all of the data that was saved during capture.
				Thread playThread = new Thread(new PlayThread());
      			playThread.start();
				}
			}
		);
		
		getContentPane().add(playBtn);	
		
//		slider.addChangeListener(new ChangeListener() {
//			public void stateChanged(ChangeEvent ev) {
//			gain=slider.getValue();
//			}
//		}
//		);
    	
//    	getContentPane().add(slider);	
    	
    	outputText.setEditable(false);
    	outputText.setColumns(60); 
    	getContentPane().add(outputText);
    		
		getContentPane().setLayout(new FlowLayout());
		setTitle("Capture/Playback");
		setDefaultCloseOperation(EXIT_ON_CLOSE);
		pack(); 
		setVisible(true);
	}

	private void initCapture(){
		try{
			if(wavFile!=null){
				wavFile.delete();
			}
			wavFile = new File("microphone.wav");
			wavFile.deleteOnExit();
			wavFileHandler=new RandomAccessFile(wavFile,"rw");
		}catch(NullPointerException e){
		}catch(FileNotFoundException e){
			e.printStackTrace();
		}
		initWav();
		captureAudio=true;
	}
	
	public void initWav(){
		byte headerPlaceHolder[]="00000000000000000000000000000000000000000000".getBytes();				
		try	{
			wavFileHandler.write(headerPlaceHolder,0,44);
		}catch(IOException e){
			e.printStackTrace();
		}
	}
	
	public void messageReceived(int dest_addr,Message msg){
		if(captureAudio){
			MicMsg data = (MicMsg)msg;
			dataCapture(data);		
		}
	}
	
	private void dataCapture(MicMsg micMsg){
//		int expandData=0;
		if(firstTime){
			bufferNum=micMsg.get_bufferNum();
			bufferSize=micMsg.totalSize_data()/micMsg.elementSize_data();
			missedPktNum=0;
			receivedPktNum=0;
			firstTime=false;
		}
		++receivedPktNum;
		if(bufferNum==micMsg.get_bufferNum()){
			++bufferNum;
			for(int i=0;i<bufferSize;++i){
//				expandData=micMsg.getElement_data(i)<<2;
				try{
//					if(expandData<=255){
//	      				wavFileHandler.write(0);
//					}else{
//		        		wavFileHandler.write(expandData);
//					}
//	        		wavFileHandler.write(expandData);
	        		wavFileHandler.write(micMsg.getElement_data(i));
 				}catch(IOException e){
					e.printStackTrace();
				}
 			}
		}else{
			for(long i=bufferNum;i<micMsg.get_bufferNum();++i){
				missedPktNum++;
			}
			bufferNum=micMsg.get_bufferNum()+1;
		}
	}
	
	private void endCapture(){
		captureAudio = false;
		firstTime = true;
		setHeader((int)(bufferSize*receivedPktNum));
		DisplayText("Received pakets: "+receivedPktNum+" Missed packets: "+missedPktNum+" Total loss: "+((double)missedPktNum/(double)(receivedPktNum+missedPktNum))*100+" %");
	}
	
	private void setHeader(int NumSamples)
	{
		byte ChunkID[]="RIFF".getBytes();
		long ChunkSize;
		byte _Format[]="WAVE".getBytes();
		
		byte Subchunk1ID[]="fmt ".getBytes();
		long Subchunk1Size=16;
		int AudioFormat=1;
		int NumChannels=1;
		long SampleRate=8928;
		long ByteRate;
		int BlockAlign;
		int BitsPerSample=8;
		
		byte Subchunk2ID[]="data".getBytes();
		long Subchunk2Size;
		
		Subchunk2Size=NumSamples*NumChannels*BitsPerSample/8;
		ChunkSize=36+Subchunk2Size;
		ByteRate=SampleRate*NumChannels*BitsPerSample/8;
		BlockAlign=NumChannels*BitsPerSample/8;
		
		try{
			// write the wav file header
			wavFileHandler.seek(0);
			wavFileHandler.write(ChunkID,0,4);
			wavFileHandler.write(intToByteArray((int)ChunkSize), 0, 4);
			wavFileHandler.write(_Format,0,4);
			wavFileHandler.write(Subchunk1ID,0,4);
			wavFileHandler.write(intToByteArray((int)Subchunk1Size), 0, 4);
			wavFileHandler.write(shortToByteArray((short)AudioFormat), 0, 2);
			wavFileHandler.write(shortToByteArray((short)NumChannels), 0, 2);
			wavFileHandler.write(intToByteArray((int)SampleRate), 0, 4);
			wavFileHandler.write(intToByteArray((int)ByteRate), 0, 4);
			wavFileHandler.write(shortToByteArray((short)BlockAlign), 0, 2);
			wavFileHandler.write(shortToByteArray((short)BitsPerSample), 0, 2);
			wavFileHandler.write(Subchunk2ID,0,4);
			wavFileHandler.write(intToByteArray((int)Subchunk2Size), 0, 4);
			wavFileHandler.close();
		}catch(IOException e){
			e.printStackTrace();
		}
	}
	
	class PlayThread extends Thread {
		FileInputStream inputWavFile=null;
		AudioInputStream audioInputStream=null;
		AudioFormat format=null;
		SourceDataLine auline=null;
		DataLine.Info info=null;
		int nBytesRead = 0, amplifiedData=0;
//		byte[] abData = new byte[2];
		byte[] abData = new byte[1000];
		public	void run(){
			try{
				inputWavFile = new FileInputStream("microphone.wav");
			}catch (FileNotFoundException e){
				e.printStackTrace();
			}
			try{
				audioInputStream = AudioSystem.getAudioInputStream(inputWavFile);
			}catch(UnsupportedAudioFileException e){
			}catch(IOException e){
				e.printStackTrace();
			}
			format = audioInputStream.getFormat();
			info = new DataLine.Info(SourceDataLine.class, format);
			try{
				auline = (SourceDataLine) AudioSystem.getLine(info);
				auline.open(format);
			}catch(LineUnavailableException e){
					e.printStackTrace();
			}	
			auline.start();
			try{
				while ((nBytesRead = audioInputStream.read(abData, 0, abData.length))!=-1){
					if (nBytesRead >= 0){
//						amplifiedData=(byteArrayToInt(abData,0))<<gain;
//						if(amplifiedData <= 255){
//		    				abData[0]=0;
//      				}else{
//		     				abData[0]=(byte)(amplifiedData>>8);
//        				}
//        				abData[1]=(byte)amplifiedData;
						auline.write(abData, 0, nBytesRead);
					}
				}
			}catch(IOException e){
				e.printStackTrace();
			}
		auline.drain();
		auline.close();
		}
	}
	
	private static byte[] intToByteArray(int i)
	{
		byte[] b = new byte[4];
		b[0] = (byte) (i & 0x00FF);
		b[1] = (byte) ((i >> 8) & 0x000000FF);
		b[2] = (byte) ((i >> 16) & 0x000000FF);
		b[3] = (byte) ((i >> 24) & 0x000000FF);
		return b;
	}

	private static byte[] shortToByteArray(short data)
	{
		return new byte[]{(byte)(data & 0xff),(byte)((data >>> 8) & 0xff)};
	}
	
	private int byteArrayToInt(byte[] b, int offset) {
		int value = 0;
		for (int i = 0; i < 2; i++) {
			int shift = (2 - 1 - i) * 8;
			value += (b[i + offset] & 0x000000FF) << shift;
		}
		return value;
	}
	
	private void DisplayText(String text) {
		if (text != null){
			outputText.setText(text);
		}
	}
}

