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
* Author: Paczolay Denes
* Author: Gosztolya Gabor
*/

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
	
	public static final boolean is8bits			= false;

	PhoenixSource phoenix;
	MoteIF mif;
	
	boolean captureAudio = false;
	boolean firstTime = true;
	long missedPktNum,bufferNum,bufferSize,receivedSamples,receivedPktNum;
	
	File wavFile;
	RandomAccessFile wavFileHandler;
	TextField outputText;
	TextField currentGain;

			
	public static void main(String args[]){
		new Capture();
	}

	public Capture(){
	
		final JButton captureBtn = new JButton("Capture");
		final JButton stopBtn = new JButton("Stop");
		final JButton playBtn = new JButton("Playback");

		outputText = new TextField();
		currentGain = new TextField();
			
		phoenix=BuildSource.makePhoenix("serial@com26:921600", PrintStreamMessenger.err);
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

    	outputText.setEditable(false);
    	outputText.setColumns(60); 
    	getContentPane().add(outputText);
    		
    	currentGain.setEditable(false);
    	currentGain.setColumns(60); 
    	getContentPane().add(currentGain);

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
			//wavFile.deleteOnExit();
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
		if(firstTime){
			bufferNum=micMsg.get_bufferNum();
			bufferSize=micMsg.totalSize_data()/micMsg.elementSize_data();
			receivedSamples = 0;
			missedPktNum=0;
			receivedPktNum=0;
			firstTime=false;
		}
		++receivedPktNum;

		// to handle missed packages
		while (bufferNum < micMsg.get_bufferNum()) {
			
			missedPktNum++;
			bufferNum++;
		}
		bufferNum++;
		
		try{
			if (is8bits) {
				for(int i=0;i<bufferSize;++i){
					wavFileHandler.write(micMsg.getElement_data(i));
				}
				receivedSamples += bufferSize;
			}
			else {
				long anum = bufferSize / 5;
				int abase = 0;
				short[] ares = new short[4];
				short fifth;
				for (int i = 0; i < anum; i++) {
					ares[0] = micMsg.getElement_data(abase + 0);
					ares[1] = micMsg.getElement_data(abase + 1);
					ares[2] = micMsg.getElement_data(abase + 2);
					ares[3] = micMsg.getElement_data(abase + 3);
					fifth = micMsg.getElement_data(abase + 4);
					ares[0] |= (fifth &   3) << 8;
					ares[1] |= (fifth &  12) << 6;
					ares[2] |= (fifth &  48) << 4;
					ares[3] |= (fifth & 192) << 2;
					abase += 5;
					for (int j = 0; j < 4; j++) {
						int atmp = ares[j];
						ares[j] = (short)((atmp << 6) - 32768);
						wavFileHandler.writeShort(Short.reverseBytes(ares[j]));
					}
				}
				receivedSamples += anum * 4;
 			}
 			currentGain.setText("The current gain is: " + micMsg.get_gainVal());
		}
		catch (IOException e) {
			e.printStackTrace();
		}
	}
	
	private void endCapture(){
		captureAudio = false;
		firstTime = true;
		setHeader((int)(receivedSamples));
		outputText.setText("Total received packets: "+(receivedPktNum+missedPktNum)+" Total missed packets: "+missedPktNum+" Total loss: "+((double)missedPktNum/(double)(receivedPktNum+missedPktNum))*100+" %");
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
		long SampleRate=17723;
		long ByteRate;
		int BlockAlign;
		int BitsPerSample= is8bits ? 8 : 16;
		
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
	
}