import java.awt.Rectangle;
import java.awt.event.AdjustmentEvent;
import java.awt.event.AdjustmentListener;

import javax.swing.JTextField;


public class MyAction implements AdjustmentListener, DataBase {

	TimeLineDraw timeLine = SnifferGraph.timeLine;						
	JTextField[] verticalText = SnifferGraph.verticalText;				
	
	public static boolean first = true;									
	
	public void adjustmentValueChanged(AdjustmentEvent en) {			
		if(first){
			first = false;
		}
		else{				
			Rectangle re = timeLine.getBounds();				
			timeLine.setBounds(TIMELINE_X-en.getValue(), re.y, re.width, re.height); 		
			int k = 122;
			for(int i = 0; i<verticalText.length; i++){									
				Rectangle vt = verticalText[i].getBounds();							
				verticalText[i].setBounds(k-en.getValue(), vt.y, vt.width, vt.height);	
				k+=70;
			}
		}
	}

}
