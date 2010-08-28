/** Copyright (c) 2010, University of Szeged
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
 * 
 * @author Nyilas Sandor Karoly, Nemeth Gabor
 * 
 *This Class specifies how to draw  timeLine and VerticalText and DataPanel
 *
 */

package org.szte.wsn.SnifferGraph;
import java.awt.Rectangle;
import java.awt.event.AdjustmentEvent;
import java.awt.event.AdjustmentListener;
import java.util.ArrayList;
import javax.swing.JPanel;
import javax.swing.JTextField;

public class MyAction implements AdjustmentListener, DataBase {

	TimeLineDraw timeLine = SnifferGraph.timeLine;
	ArrayList<JTextField> osTime = SnifferGraph.osTime;
	ArrayList<JTextField> moteTime = SnifferGraph.moteTime;
	JTextField osTimeText = SnifferGraph.osTimeText;
	JTextField moteTimeText = SnifferGraph.moteTimeText;
	ArrayList<JPanel> dataPanel = SnifferGraph.dataPanel;
	ArrayList<Packages> motes = SnifferGraph.motes;
	
	public static boolean first = true;									
	/**
	 * @param: AdjustmentEvent  this gives the moving value
	 */
	public void adjustmentValueChanged(AdjustmentEvent en) {			
		if(first){
			first = false;
		}
		else{				
			Rectangle re = timeLine.getBounds();				
			timeLine.setBounds(TIMELINE_X-en.getValue(), re.y, re.width, re.height); 		
			int k = 100;
			for(int i = 0; i<osTime.size(); i++){									
				Rectangle vt = osTime.get(i).getBounds();							
				osTime.get(i).setBounds(k-en.getValue(), vt.y, vt.width, vt.height);	
				k+=100;
			}
			
			k = 100;
			for(int i = 0; i<moteTime.size(); i++){									
				Rectangle vt = moteTime.get(i).getBounds();							
				moteTime.get(i).setBounds(k-en.getValue(), vt.y, vt.width, vt.height);	
				k+=100;
			}
			k=100;
			
			for(int i = 0; i<dataPanel.size(); i++){
				Rectangle rt = dataPanel.get(i).getBounds();
				dataPanel.get(i).setBounds(motes.get(i).getFirstPos()-en.getValue(), rt.y, rt.width, rt.height);
				if(motes.get(i).getFirstPos()-en.getValue()<90){
					//dataPanel.get(i).setVisible(false);				///ez még nagyon nem stabul SANYA A KURVA ANYÁD
				}
				else{
					//dataPanel.get(i).setVisible(true);
				}
				k+=100;
			}
			
			
			Rectangle mr = moteTimeText.getBounds();
			moteTimeText.setBounds(30-en.getValue(), mr.y, mr.width, mr.height);
			mr = osTimeText.getBounds();
			osTimeText.setBounds(30-en.getValue(), mr.y, mr.width, mr.height);
		}
	}
}
