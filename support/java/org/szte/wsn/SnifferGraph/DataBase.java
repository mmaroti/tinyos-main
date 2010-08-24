package org.szte.wsn.SnifferGraph;
import java.awt.Dimension;
import java.awt.Toolkit;
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