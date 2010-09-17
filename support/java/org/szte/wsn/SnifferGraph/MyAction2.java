package org.szte.wsn.SnifferGraph;

import java.awt.event.AdjustmentEvent;
import java.awt.event.AdjustmentListener;
import java.util.ArrayList;

import javax.swing.JPanel;

public class MyAction2 implements AdjustmentListener, DataBase {

	public static ArrayList<JPanel> dataPanel = SnifferGraph.dataPanel2;
	ArrayList<Packages> motes = SnifferGraph.motes;
	public static boolean first = true;
	
	public void adjustmentValueChanged(AdjustmentEvent e) {
		if(first){
			first = false;
		}
		else{				
			for(int i = 0; i < dataPanel.size(); i++){
				dataPanel.get(i).setBounds(dataPanel.get(i).getBounds().x, motes.get(i).firstPosVerical-e.getValue(),
						dataPanel.get(i).getBounds().width, dataPanel.get(i).getBounds().height);
			}
		}
	}

}
