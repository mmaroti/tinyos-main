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
	JTextField[] verticalText = SnifferGraph.verticalText;
	ArrayList<JPanel> dataPanel = SnifferGraph.dataPanel;
	
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
			int k = 122;
			for(int i = 0; i<verticalText.length; i++){									
				Rectangle vt = verticalText[i].getBounds();							
				verticalText[i].setBounds(k-en.getValue(), vt.y, vt.width, vt.height);	
				k+=100;
			}
			k=100;
			for(int i = 0; i<dataPanel.size(); i++){
				Rectangle rt = dataPanel.get(i).getBounds();
				dataPanel.get(i).setBounds(k-en.getValue(), rt.y, rt.width, rt.height);
				if(k-en.getValue()<90){
					dataPanel.get(i).setVisible(false);
				}
				else{
					dataPanel.get(i).setVisible(true);
				}
				k+=100;
			}
		}
	}
}
