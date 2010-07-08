import java.awt.Rectangle;
import java.awt.event.AdjustmentEvent;
import java.awt.event.AdjustmentListener;

import javax.swing.JTextField;


public class MyAction implements AdjustmentListener, DataBase {

	TimeLineDraw timeLine = SnifferGraph.timeLine;						//Getting the time line draw
	JTextField[] verticalText = SnifferGraph.verticalText;				//and the numbers 
	
	public static boolean first = true;									//First run value
	
	public void adjustmentValueChanged(AdjustmentEvent en) {			//On every scroll bar change this will be called
		if(first){
			first = false;
		}
		else{				
			Rectangle re = timeLine.getBounds();				
			timeLine.setBounds(TIMELINE_X-en.getValue(), re.y, re.width, re.height); 	//Set the timeLine own  panel size	
			int k = 22;
			for(int i = 0; i<verticalText.length; i++){									//write 100 numbers horizontally
				Rectangle vt = verticalText[i].getBounds();							
				verticalText[i].setBounds(k-en.getValue(), vt.y, vt.width, vt.height);	//dynamic positions
				k+=70;
			}
		}
	}

}
