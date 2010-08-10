package org.szte.wsn.SnifferGraph;
import java.awt.Rectangle;
import java.awt.event.AdjustmentEvent;
import java.awt.event.AdjustmentListener;
import java.util.ArrayList;

import javax.swing.JPanel;
import javax.swing.JTextField;

/**
 * 
 * @author Nyilas Sandor Karoly, Nemeth Gabor
 * 
 *This Class specifies how to draw  timeLine and VerticalText and DataPanel
 *
 */
public class MyAction implements AdjustmentListener, DataBase {

	TimeLineDraw timeLine = SnifferGraph.timeLine;						
	
	ArrayList<JTextField> osTime = SnifferGraph.osTime;
	ArrayList<JTextField> moteTime = SnifferGraph.moteTime;
	
	JTextField osTimeText = SnifferGraph.osTimeText;
	JTextField moteTimeText = SnifferGraph.moteTimeText;
	
	ArrayList<JPanel> dataPanel = SnifferGraph.dataPanel;
	ArrayList<Mote> motes = SnifferGraph.motes;
	
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
					dataPanel.get(i).setVisible(false);
				}
				else{
					dataPanel.get(i).setVisible(true);
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
