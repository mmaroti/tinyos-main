package org.szte.wsn.SnifferGraph;
import java.awt.Dimension;
import java.awt.Toolkit;
/**
 * @author Nemeth Gabor, Nyilas Sandor Karoly
 *This class contains interface for specifying the default screen coordinates and button sizes
 * 
 * 
 * version: 0.01b
 */

public interface DataBase {
	
	Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();			
	
	public final int screenHeight = screenSize.height;							
	public final int screenWidth = screenSize.width;
	
	public final int JCHECKBOX_FIRST_SIZE = 200;
	public final int HORIZONTAL_JCB_SPACE = screenHeight > 768 ? 70 : 40;
	public final int JCHECKBOX_SIZE_X = 80;
	public final int JCHECKBOX_SIZE_Y = 20;
	
	public final int BUTTON_WIDTH = screenHeight > 768 ? 75 : 70;				
	public final int BUTTON_HEIGHT = screenHeight > 768 ? 26 : 24;
	
	public final int STATUSBAR_HEIGHT = screenHeight - 71;						
	
	public final int TIMELINE_X = 130;											
	public final int TIMELINE_Y = 70;											
	public final int TIMELINE_WIDTH = screenWidth-53;					
	public final int TIMELINE_HEIGHT = 12;
	
	public final int BACKG_PANEL_X = 30;										
	public final int BACKG_PANEL_Y = 50;
	public final int BACKG_WIDTH = screenWidth-40;
	public final int BACKG_HEIGHT = screenHeight-170;
	
	public final int DATAPANEL_FIRST_SIZE = 100;
	public final int DATAPANEL_SPACE = 100;
	public final int DATAPANEL_X = 70;
	public final int DATAPANEL_Y = JCHECKBOX_FIRST_SIZE + HORIZONTAL_JCB_SPACE;
	public final int DATAPANEL_SIZE_X = 71;
	public final int DATAPANEL_SIZE_Y = 40;
	
}