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
* 
* This program is created to be the GUI for the Sniffer Wireless Project
* 
* 
* @version 0.3
*/

package org.szte.wsn.SnifferGraph;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Event;
import java.awt.FlowLayout;
import java.awt.Frame;
import java.awt.GridLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.io.File;
import java.util.ArrayList;

import javax.swing.BorderFactory;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JPanel;
import javax.swing.JScrollBar;
import javax.swing.JSeparator;
import javax.swing.JTabbedPane;
import javax.swing.JTextField;
import javax.swing.KeyStroke;
import javax.swing.SwingConstants;

import org.szte.wsn.SnifferGraph.config.*;

public class SnifferGraph implements DataBase{	
	
	/**
	 *Global value declarations for 
	 */
	
	private static final long serialVersionUID = 1L;
	
	JFrame snfG = new JFrame((String)Labels.frame_name);
	
	public static ArrayList<JPanel> dataPanel = new ArrayList<JPanel>();		//normal nezet
	public static ArrayList<JPanel> dataPanel2 = new ArrayList<JPanel>();		//2odik nezet
	public static ArrayList<Packages> motes = new ArrayList<Packages>();
	public static ArrayList<Color> colorlist = new ArrayList<Color>();
	public static ArrayList<String> colors = new ArrayList<String>();		
	public static ArrayList<String> dataTypes = new ArrayList<String>();//páros típus, páratlan szin
	
	//public static XmlRead xmlRead = new XmlRead();
	JTabbedPane tab = new JTabbedPane();
	JPanel mainPanel = new JPanel(new BorderLayout(10, 10));
	JPanel southPanel = new JPanel(new FlowLayout(FlowLayout.LEFT));
	JPanel northPanel = new JPanel(new FlowLayout(FlowLayout.RIGHT));
	JPanel leftPanel = new JPanel(new FlowLayout(FlowLayout.RIGHT));
	public static JPanel centerPanel = new JPanel(new BorderLayout(10, 10)); //Composite fulhoz a panel
	public static JPanel centerPanel2 = new JPanel(new BorderLayout(10, 10)); //Data fulhoz a panel
	public static JPanel centerPanel3 = new JPanel(new BorderLayout(10, 10)); //Timing fulhoz a panel
	public static JPanel forBox = new JPanel();

	
	public static JPanel backgPanel = new JPanel(); //Composite ful
	public static JPanel backgPanel2 = new JPanel(); //Data ful
	public static JPanel backgPanel3 = new JPanel(); //Timing ful
	
	JMenuBar menubar = new JMenuBar();
    JMenu file = new JMenu("File");
    JMenu conf = new JMenu("Configuration");
    JMenu sniffer = new JMenu("Sniffer");
    JMenu help = new JMenu("Help");
    
    JMenuItem fileClose = new JMenuItem(Labels.menu_exit);
    JMenuItem about = new JMenuItem(Labels.menu_about);
    JMenuItem openConfiguration = new JMenuItem(Labels.menu_open_conf);	
    JMenuItem newConfiguration = new JMenuItem(Labels.menu_create);
    JMenuItem save = new JMenuItem(Labels.menu_save);
    JMenuItem open = new JMenuItem(Labels.menu_load_packages);
    JMenuItem help_menu = new JMenuItem(Labels.menu_help);
    JMenuItem save_as = new JMenuItem(Labels.menu_saveas);
    JMenuItem start_menu = new JMenuItem(Labels.menu_start);
    JMenuItem stop_menu = new JMenuItem(Labels.menu_stop);
    JMenuItem set_chn = new JMenuItem(Labels.menu_setchn);
    JMenuItem set_port = new JMenuItem(Labels.menu_setport);
    
    JButton start = new JButton(Labels.button_start);
   
    JButton clear = new JButton(Labels.button_clear);
    
    JTextField startStop = new JTextField(Labels.txfield_pause);
    JTextField recived = new JTextField(Labels.txfield_recived);
    JTextField channelT = new JTextField(Labels.txfield_channelT);
    static JTextField receivedPack = new JTextField("1");
    JTextField channel = new JTextField("1L");
    JTextField loadedFileT = new JTextField(Labels.txfield_loadedFileT);
    JTextField loadedFile = new JTextField("xmlRead.filename");
    JTextField saved = new JTextField("xmlRead.filename");
    
    public static ArrayList<JTextField> osTime = new ArrayList<JTextField>();
    public static ArrayList<JTextField> moteTime = new ArrayList<JTextField>();
    
    public static JTextField osTimeText = new JTextField("OS Time:");
	public static JTextField moteTimeText = new JTextField("packages Time:");
    
    public static TimeLineDraw timeLine = new TimeLineDraw();
    
    public static JScrollBar scrollbar = new JScrollBar(JScrollBar.HORIZONTAL);
 
    
    public static ArrayList<JCheckBox> devices = new ArrayList<JCheckBox>();
    
    JFileChooser fc = new JFileChooser();
    
    ToolBarButton exit_toolbar;
    ToolBarButton about_toolbar;
    ToolBarButton creat_toolbar;
    ToolBarButton load_toolbar;
    ToolBarButton open_conf_toolbar;
    ToolBarButton save_toolbar;
    
	/**
	 * This function gives information about the start button status.
	 */
	private void startStopProcess() {					
		// TODO Auto-generated method stub
		if(start.getActionCommand().equals(Labels.button_start)){	
			start.setText(Labels.button_stop);
			startProcess();
		}
		else{
			start.setText(Labels.button_start);						
			stopProcess();
		}
	}
	
	/**
	 * This function gives information about the process is stopped.
	 */
	private void stopProcess() {						
		System.out.println("Process leált.");
		startStop.setText(Labels.txfield_pause);
		// TODO Auto-generated method stub
		
	}
	
	/**
	 * This function gives information about the process is working.
	 */
	private void startProcess() {						
		System.out.println("Process indult.");
		startStop.setText(Labels.txfield_working);
		// TODO Auto-generated method stub
		
	}
	
	/**
	 * This function resets the current session.
	 */
	private void clearWork() {							
		System.out.println("clear");
		stopProcess();
		
		baseSetings();
		// TODO Auto-generated method stub
		
	}
	
	/**
	 * This function loads a saved session.
	 */
    protected void openHistory() {						
    	System.out.println("open");
    	int returnVal = fc.showOpenDialog(openConfiguration);	
		if (returnVal == JFileChooser.APPROVE_OPTION) {
            @SuppressWarnings("unused")
			File file = fc.getSelectedFile();			
            //open a history file
        } else {
            //cancel
        }
		// TODO
		
	}
    
	/**
	 * This function crates a new configuration file.
	 */
	protected void createConfiguration() {						
		System.out.println("createop");
		// TODO
		new CreateConfGUI();
	}
	
	/**
	 * This function saves the current session.
	 */
	protected void saveHistory() {						
		System.out.println("save");
		int returnVal = fc.showSaveDialog(save);
        if (returnVal == JFileChooser.APPROVE_OPTION) {
            @SuppressWarnings("unused")
			File file = fc.getSelectedFile();
            //save history
        } else {
            //cancel
        }
		// TODO Auto-generated method stub
	}
	
	/**
	 * This function opens a configuration xml file.
	 */
	private void openConfiguration() {							
		System.out.println("openop");
		int returnVal = fc.showOpenDialog(openConfiguration);
		if (returnVal == JFileChooser.APPROVE_OPTION) {
            @SuppressWarnings("unused")
			File file = fc.getSelectedFile();
            //open the XML file
        } else {
        	//cancel
        }
		// TODO Auto-generated method stub
	}
	/**
	 * Class constructor
	 */
	public SnifferGraph() {
		
		backgPanel.setLayout(null);
        backgPanel.setBackground(Color.white);						
        backgPanel.setBorder(BorderFactory.createEtchedBorder());
        backgPanel2.setLayout(null);
        backgPanel2.setBackground(Color.white);						
        backgPanel2.setBorder(BorderFactory.createEtchedBorder());
        backgPanel3.setLayout(null);
        backgPanel3.setBackground(Color.white);						
        backgPanel3.setBorder(BorderFactory.createEtchedBorder());	

        scrollbar.addAdjustmentListener(new MyAction());			
        scrollbar.setMaximum(0);
        
        menuCreat();												
        editables();										
        baseSetings();
        creatToolBar();
        createActionListeners();
        adding();											
        setTooltips();													
        createTimeLine();
		//createBoxes();
        setMnemonics();
        makeAcolors();
        
        snfG.setSize(800, 600);					
        snfG.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
        snfG.setVisible(true);
   }
	
	private void makeAcolors() {
		colorlist.add(Color.blue);
		colorlist.add(Color.cyan);
		colorlist.add(Color.darkGray);
		colorlist.add(Color.gray);
		colorlist.add(Color.green);
		colorlist.add(Color.lightGray);
		colorlist.add(Color.magenta);
		colorlist.add(Color.orange);
		colorlist.add(Color.pink);
		colorlist.add(Color.red);
		colorlist.add(Color.yellow);
		colorlist.add(Color.black);
	}

	private void creatToolBar() {
		exit_toolbar = new ToolBarButton("./org/szte/wsn/SnifferGraph/icon/Exit.png");
		exit_toolbar.setToolTipText("Exit");
		
		about_toolbar = new ToolBarButton("./org/szte/wsn/SnifferGraph/icon/About.png");
		about_toolbar.setToolTipText("About to Sniffer");
	    creat_toolbar = new ToolBarButton("./org/szte/wsn/SnifferGraph/icon/Create-Configuration.png");
	    creat_toolbar.setToolTipText("Creat a new conf file");
	    load_toolbar = new ToolBarButton("./org/szte/wsn/SnifferGraph/icon/Load-Packages.png");
	    load_toolbar.setToolTipText("Load a history");
	    open_conf_toolbar = new ToolBarButton("./org/szte/wsn/SnifferGraph/icon/Open-Configuration.png");
	    open_conf_toolbar.setToolTipText("Open a conf file");
	    save_toolbar = new ToolBarButton("./org/szte/wsn/SnifferGraph/icon/Save.png");
	    save_toolbar.setToolTipText("Save a history");
	}
	
	/**
	 * Function will used by checkboxes to know what data visible
	 * @param event
	 */
	/*private void switching(ActionEvent event) {
		System.out.println(event.getActionCommand()); 
		Process.reDraw();
		// TODO Auto-generated method stub
		scrollbar.setValue(scrollbar.getValue()+1);
		scrollbar.setValue(scrollbar.getValue()-1);
	}*/

	/**
	 * This function specifies the Hot keys and the Mnemonic chars in JMenubar .
	 */
	private void setMnemonics() {
		fileClose.setMnemonic('x');
        fileClose.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_X,Event.CTRL_MASK));
        open.setMnemonic('o');
        open.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_O,Event.CTRL_MASK));
        save.setMnemonic('s');
        save.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_S,Event.CTRL_MASK));
        openConfiguration.setMnemonic('O');
        openConfiguration.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_O,Event.SHIFT_MASK));
        newConfiguration.setMnemonic('C');
        newConfiguration.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_C,Event.SHIFT_MASK));
        about.setMnemonic('A');
        about.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_A,Event.ALT_MASK));

	}
	
	/**
	 * This function draws the JtextFields under the TimeLine.
	 */
	private void createTimeLine() {	
		osTimeText.setBorder(null);
		osTimeText.setBackground(null);
		osTimeText.setBounds(30, 40, 55, 30);
		
		moteTimeText.setBorder(null);
		moteTimeText.setBackground(null);
		moteTimeText.setBounds(30, 110, 74, 30);
		
		backgPanel.add(osTimeText);
		backgPanel.add(moteTimeText);
		
		for(int i = 0; i < 100; i++){
			osTime.add(new JTextField(Process.whatTimeIsNow()));
		}
		int k = 100;
		for(int i = 0;i < 100; i++){
			osTime.get(i).setBounds(k, 40, 75, 30);
			k+=100;
			osTime.get(i).setBorder(null);
			backgPanel.add(osTime.get(i));								
		}
		for(int i = 0; i < 100; i++){
			moteTime.add(new JTextField(Process.whatMoteTimeNow()));
		}
		k = 100;
		for(int i = 0;i < 100; i++){
			moteTime.get(i).setBounds(k, 110, 85, 30);
			k+=100;
			moteTime.get(i).setBorder(null);
			backgPanel.add(moteTime.get(i));								
		}
		
	}
	
	/**
	 * This function specifies which objects added to the file menu.
	 */
	private void menuCreat() {
        file.add(open);
        file.add(save);
        file.add(save_as);
        file.add(new JSeparator());
        file.add(fileClose);
        
        menubar.add(file);
        
        conf.add(newConfiguration);
        conf.add(openConfiguration);
        
        menubar.add(conf);
        
        sniffer.add(start_menu);
        sniffer.add(stop_menu);
        sniffer.add(set_chn);
        sniffer.add(set_port);
        
        menubar.add(sniffer);
        
        help.add(about);
        help.add(help_menu);
        
        menubar.add(help);
        
        snfG.setJMenuBar(menubar);										
	}
	
	/**
	 * This function  specifying tool tips for objects.
	 * The text displays when the cursor lingers over the component.
	 */
	private void setTooltips() {
		newConfiguration.setToolTipText(Labels.tooltips_create_conf);					
        open.setToolTipText(Labels.tooltips_open);
        openConfiguration.setToolTipText(Labels.tooltips_open_conf);
        save.setToolTipText(Labels.tooltips_save);
        start.setToolTipText(Labels.tooltips_start);
        about.setToolTipText(Labels.tooltips_about);
        clear.setToolTipText(Labels.tooltips_clear);
        fileClose.setToolTipText(Labels.tooltips_fileClose);
	}
	
	/**
	* This Older function  specified where to align exactly the objects on the workspace.
	*/
	private void baseSetings() {
		for(int i = 0; i<dataPanel.size();i++){
			dataPanel.get(i).setVisible(false);		//főnézet tiszítása
			backgPanel.remove(dataPanel.get(i));
		}
		int dataPanelLength = dataPanel.size();
		for(int i = 0; i<dataPanelLength; i++){
			dataPanel.remove(0);
		}
		
		for(int i = 0; i<dataPanel2.size();i++){
			dataPanel2.get(i).setVisible(false);	//második nézet tisztítása
			backgPanel2.remove(dataPanel2.get(i));
		}
		dataPanelLength = dataPanel2.size();
		for(int i = 0; i<dataPanelLength; i++){
			dataPanel2.remove(0);
		}
		
		for(int i=0; i<devices.size();i++){			//checkboxok üritése
			devices.get(i).setText("<none>");
			devices.get(i).setEnabled(true);
			devices.get(i).setVisible(false);
		}
		
		int moteSize = motes.size();
		for(int i = 0; i<moteSize-1; i++) motes.remove(1);
		
		scrollbar.setMaximum(DataBase.screenWidth-500);
		scrollbar.setValue(0);
	}

	/**
	 * This function  adds objects to southPanel , northPanel , backgPanel , centerPanel or mainPanel.
	 * To be aligned on our workspace
	 */
	private void adding() {
		
		southPanel.add(startStop);
		southPanel.add(recived);
		southPanel.add(receivedPack);
		southPanel.add(channelT);
		southPanel.add(channel);
		southPanel.add(loadedFileT);
		southPanel.add(loadedFile);
		
		northPanel.add(load_toolbar);
		northPanel.add(save_toolbar);
		northPanel.add(creat_toolbar);
		northPanel.add(open_conf_toolbar);
		northPanel.add(about_toolbar);
		northPanel.add(exit_toolbar);
		northPanel.add(start);
		northPanel.add(clear);
		
		timeLine.setBounds(TIMELINE_X, TIMELINE_Y, TIMELINE_WIDTH+1000000, TIMELINE_HEIGHT);		
	    timeLine.setBackground(Color.white);
		
		backgPanel.add(timeLine);
		//int k = DATAPANEL_FIRST_SIZE;
		/*for(int i=0;i<dataPanel.size();i++){
			dataPanel.get(i).setBounds(k, DATAPANEL_Y, DATAPANEL_SIZE_X, ((xmlRead.number+3)*DATAPANEL_SIZE_Y));
			backgPanel.add(dataPanel.get(i));
			k+=DATAPANEL_SPACE;
		}*/
		
		//devices.add(new JCheckBox("Device01"));
		for(int i = 0; i<=10; i++){
			devices.add(new JCheckBox("<none>"));
			devices.get(devices.size()-1).setVisible(false);
		}
		forBox.setLayout(new GridLayout(13,10,5,5));
		forBox.setBounds(0, 0, 100, 800);
		//forBox.add(new JPanel());
		forBox.add(new JPanel());
		forBox.add(new JPanel());
		forBox.add(new JPanel());
		forBox.add(new JPanel());
		forBox.add(new JPanel());
		for(int i = 5; i < 12; i++){
			forBox.add(devices.get(i-5));
		}
		centerPanel.add(forBox, BorderLayout.WEST);
		
		
		
		centerPanel.add(scrollbar, BorderLayout.SOUTH);

		
		centerPanel.add(backgPanel, BorderLayout.CENTER);
		centerPanel2.add(backgPanel2, BorderLayout.CENTER);
		centerPanel3.add(backgPanel3, BorderLayout.CENTER);
		
		mainPanel.add(southPanel, BorderLayout.SOUTH);
		mainPanel.add(northPanel, BorderLayout.NORTH);
		mainPanel.add(leftPanel, BorderLayout.WEST);
		
		tab.add("Composite",centerPanel);
		tab.add("Data",centerPanel2);
		tab.add("Timing", centerPanel3);
		tab.add("RandomGirl", new JButton("babe"));
		mainPanel.add(tab, BorderLayout.CENTER);
		
		snfG.add(mainPanel);
	}
	
	
	/**
	 * This function  specifying Action Listeners.
	 */
	private void createActionListeners() {						
		// TODO Auto-generated method stub
		exit_toolbar.addActionListener(new ActionListener() {			
            public void actionPerformed(ActionEvent event) {
            	System.exit(0);
            }
		});
		fileClose.addActionListener(new ActionListener() {			
            public void actionPerformed(ActionEvent event) {
            	System.exit(0);
            }
		});
        
        about.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent event) {
            	new About();
            }
		});
        
        start.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent event) {
                startStopProcess();
            }
		});
        
        clear.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent event) {
                clearWork();
            }
		});
        
        open.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent event) {
                openHistory();
            }
		});
        
        save.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent event) {
                saveHistory();
            }
		});
        
        newConfiguration.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent event) {
            	createConfiguration();
            }
		});
        
        openConfiguration.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent event) {
            	openConfiguration();
            }
		});
		
	}
	
	/**
	 * This function specifying which JTextfields are edit able or not
	 */
	private void editables() {											
	    startStop.setEditable(false);
	    recived.setEditable(false);
	    channelT.setEditable(false);
	    receivedPack.setEditable(false);
	    receivedPack.setHorizontalAlignment(SwingConstants.TRAILING);
	    channel.setEditable(false);
	    channel.setHorizontalAlignment(SwingConstants.TRAILING);
	    loadedFileT.setEditable(false);
        loadedFile.setEditable(false);
	}
	
	private static void addAPackge() {
		int first = RandomString.getAnumber(motes.get(motes.size()-1).stopTime);	//A két random szám lekérése a teszteléshez
		int second = RandomString.getAnumber(first);
		
		motes.add(new Packages(first, second, RandomString.getRandomMoteStrings(7)));	//Az új csomag létrehozása
		dataPanel.add(motes.get(motes.size()-1).getALilPanel(devices, colorlist));		//bekerül a listába, ami nyilván tarjta a paneleket a foablakon
		dataPanel.get(dataPanel.size()-1).setVisible(true);								//beálítjuk az ablakra
		backgPanel.add(dataPanel.get(dataPanel.size()-1));
		
		dataPanel2.add(motes.get(motes.size()-1).getABigPanel(devices, colorlist, colors, dataTypes, motes));		//a csomagot a második ablakba is hozzáadjuk, és a megfelelő listába is betesszük
		dataPanel2.get(dataPanel2.size()-1).setVisible(true);
		backgPanel2.add(dataPanel2.get(dataPanel2.size()-1));
		
		scrollbar.setMaximum((motes.get(motes.size()-1).firstPos));						//srollbar méterte igaízás
		scrollbar.setValue(scrollbar.getMaximum()-DataBase.screenWidth+400);
		receivedPack.setText(String.valueOf(Integer.parseInt(receivedPack.getText())+1));
	} 
	
	public static void main(String[] args) {
		SnifferGraph main = new SnifferGraph();
		main.snfG.setExtendedState(main.snfG.getExtendedState() | Frame.MAXIMIZED_BOTH);
		
		motes.add(new Packages(4, 10, RandomString.getRandomMoteStrings(7)));
		//motes.get(motes.size()-1).getALilPanel(devices, colorlist);
		dataPanel.add(motes.get(motes.size()-1).getALilPanel(devices, colorlist));
		dataPanel.get(dataPanel.size()-1).setVisible(true);
		backgPanel.add(dataPanel.get(dataPanel.size()-1));
		
		dataPanel2.add(motes.get(motes.size()-1).getABigPanel(devices, colorlist, colors, dataTypes, motes));		//a csomagot a második ablakba is hozzáadjuk, és a megfelelő listába is betesszük
		dataPanel2.get(dataPanel2.size()-1).setVisible(true);
		backgPanel2.add(dataPanel2.get(dataPanel2.size()-1));
		
		
		
		
		
		do{
			
			if(RandomString.getAnewInput() && main.start.getText().equals("Stop")){
				addAPackge();
			}
		}while(true);
		
	}

	
}