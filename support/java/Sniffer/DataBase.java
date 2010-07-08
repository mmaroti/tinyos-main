import java.awt.Dimension;
import java.awt.Toolkit;


public interface DataBase {
	
	Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();			//a variable that get current screen resolution
	
	public final int screenHeight = screenSize.height;							//Get height and width of the resolution
	public final int screenWidth = screenSize.width;
	
	public final int JCHECKBOX_FIRS_SIZE = 300;									//The first height value for check boxes
	public final int HORISONTAL_JCB_SPEACE = screenHeight > 768 ? 70 : 40;
	public final int JCHECKBOX_SIZE = 20;
	
	public final int BUTTON_WIDTH = screenHeight > 768 ? 75 : 70;				//Default dynamic Button Size settings
	public final int BUTTON_HEIGHT = screenHeight > 768 ? 26 : 24;
	
	public final int STATUSBAR_HEIGHT = screenHeight - 71;						//Default Status bar Height 
	
	public final int TIMELINE_X = 30;											//Default timeLine horizontal value
	public final int TIMELINE_Y = 70;											//Default timeLine vertical value
	public final int TIMELINE_WIDTH = screenWidth-53;					
	public final int TIMELINE_HEIGHT = 12;
	
	public final int BACKG_PANEL_X = 30;										//Talbe Settings
	public final int BACKG_PANEL_Y = 50;
	public final int BACKG_WIDTH = screenWidth-40;
	public final int BACKG_HEIGHT = screenHeight-170;
	
}
