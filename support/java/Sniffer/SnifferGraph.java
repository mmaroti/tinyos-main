
import java.awt.Color;
import java.awt.Frame;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;

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
import javax.swing.JTextField;
import javax.swing.SwingConstants;

/**
 * @author Nemeth Gabor, Nyilas Sandor Karoly
 * This program is created to be the GUI for the Sniffer Wireless Project
 * 
 * 
 * version: 0.01b
 */

public class SnifferGraph implements DataBase{	
	
	/**
	 * 
	 */
	private static final long serialVersionUID = 1L;
	
	JFrame snfG = new JFrame("Sniffer Package viewer");			//A snfG Frame to be a working space for this project
	
	JPanel backgPanel = new JPanel();							//A backgPanel to be a table on the working space there will take place the timeLine and other stuff
	
	JMenuBar menubar = new JMenuBar();							//A menubar to have more operation available for the users in the future
	
    JMenu file = new JMenu("File");								//The first menu on the bar								
    
    JMenuItem fileClose = new JMenuItem("Exit");				//fileClose,About,Open,Openoption,Createoption,save  menu items for the first menu on the bar.
    JMenuItem about = new JMenuItem("About");
    JMenuItem open = new JMenuItem("Load packages...");
    JMenuItem openOption = new JMenuItem("Open Option");	
    JMenuItem createOption = new JMenuItem("Creat");
    JMenuItem save = new JMenuItem("Save");
    
    JButton start = new JButton("Start");						//Some newlish main buttons to be used. 
    JButton clear = new JButton("Clear");
    
    JTextField startStop = new JTextField("Stop");				//Some TextField for the status bar
    JTextField resived = new JTextField("Resived packages: ");
    JTextField chanelT = new JTextField("Chanel: ");
    JTextField resevedPack = new JTextField("0");
    JTextField chanel = new JTextField("1L");
    JTextField loadedFileT = new JTextField("Loaded: ");
    JTextField loadedFile = new JTextField("\"none\"");
    
    public static JTextField verticalText[] = new JTextField[100];	//This is the nubmers for the timeLine this will be easily can be used for ruler
    static TimeLineDraw timeLine = new TimeLineDraw();				//This is the timeLine will be on the table
    
    JScrollBar scrollbar = new JScrollBar(JScrollBar.HORIZONTAL);	//A scrollbar to have more operations and to be more transparent if we will use this program for a "long" time maybe
    
    JCheckBox box1 = new JCheckBox();								//Furthermore checkboxes to be able choose what would we like to see on the table
    JCheckBox box2 = new JCheckBox();
    JCheckBox box3 = new JCheckBox();
    JCheckBox box4 = new JCheckBox();
    JCheckBox box5 = new JCheckBox();
    
    JFileChooser fc = new JFileChooser();				//We have a file open operation int the so early state version: ß

	private void startStopProcess() {					//To be able to start listen for the Sniffer project
		// TODO Auto-generated method stub
		if(start.getActionCommand().equals("Start")){	//Start operation
			start.setText("Stop");
			startProcess();
		}
		else{
			start.setText("Start");						//Else Stop operation  and change text on the button aswell
			stopProcess();
		}
	}

	private void stopProcess() {						//Two processes to have a sign to be able to see if the operation is halt or running
		System.out.println("Process leált.");
		startStop.setText("Pause");
		// TODO Auto-generated method stub
		
	}

	private void startProcess() {						
		System.out.println("Process indult.");
		startStop.setText("Work");
		// TODO Auto-generated method stub
		
	}

	private void clearWork() {							//An operation to get to the start
		System.out.println("clear");
		stopProcess();
		baseSetings();
		// TODO Auto-generated method stub
		
	}

    protected void openHisoty() {						//IF we try to open a configuration file
    	System.out.println("open");
    	int returnVal = fc.showSaveDialog(openOption);	
		if (returnVal == JFileChooser.APPROVE_OPTION) {
            @SuppressWarnings("unused")
			File file = fc.getSelectedFile();			//The selected one will be loaded
            //open a history file
        } else {
            //cancel
        }
		// TODO Auto-generated method stub
		
	}
    
	protected void createOption() {						//operation to generate xml configuration file
		System.out.println("createop");
		// TODO Auto-generated method stub
		
	}

	protected void saveHisoty() {						//To save the information in the current session
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

	private void openOption() {							//To be able to open configuration xml files
		System.out.println("openop");
		int returnVal = fc.showSaveDialog(openOption);
		if (returnVal == JFileChooser.APPROVE_OPTION) {
            @SuppressWarnings("unused")
			File file = fc.getSelectedFile();
            //open the XML file
        } else {
        	//cancel
        }
		// TODO Auto-generated method stub
	}

	public SnifferGraph() {
		
		snfG.setLayout(null);										//Automated arrange options get disable to have a unique designed arrange
		backgPanel.setLayout(null);
        
        createActionListeners();									//Call Create Action Listeners function
        
        backgPanel.setBackground(Color.white);						//Set the background color white because the grey is not enough good for us
        backgPanel.setBorder(BorderFactory.createEtchedBorder());	//A Stylish border

        scrollbar.addAdjustmentListener(new MyAction());			//Call the MyAction function for the Listeners
        scrollbar.setMaximum(screenWidth-110);						//Maximum value for the scroll bar
        
        menuCreat();												//Call Menu Create , Adding , Edit Tables , Base Settings , Set Tool Tips , Create Time Line function
        editables();										
        baseSetings();										
        adding();											
        setTooltips();													
        createTimeLine();
        
        snfG.setSize(screenWidth, screenHeight);					//frame should appear on the maximum screen size.
        snfG.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);		//Default Close Operation
        snfG.setExtendedState(snfG.getExtendedState() | Frame.MAXIMIZED_BOTH);	
        snfG.setVisible(true);
   }



	private void createTimeLine() {											//Number signs numbers to be the rulers
		// TODO Auto-generated method stub
		for(int i = 1; i <= 100; i++){
			verticalText[i-1] = new JTextField(i<10 ? ("00" + String.valueOf(i)) : ("0" + String.valueOf(i)));
		}
		int k = 22;
		for(int i = 0;i < 100; i++){
			verticalText[i].setBounds(k, 80, 30, 30);						//set the align on self frame
			k+=70;
			verticalText[i].setBorder(null);
			backgPanel.add(verticalText[i]);								//write i on the panel
		}
		
	}

	private void menuCreat() {
		file.add(open);
        file.add(save);
        file.add(new JSeparator());											//Separator for the unique aligns
        
        file.add(createOption);
        file.add(openOption);
        file.add(new JSeparator());											//separator
        
        file.add(about);													//Add About Menu Item for file Menu
        file.add(new JSeparator());											//separator
        file.add(fileClose);												//fileClose getting added to the file menu
        
        menubar.add(file);
        snfG.setJMenuBar(menubar);											//Get Menu bar on the frame
	}

	private void setTooltips() {
		createOption.setToolTipText("Create a new option");					//A lot of ToolTips for advice
        open.setToolTipText("Open saved packages history");
        openOption.setToolTipText("Open saved Options");
        save.setToolTipText("Save package history");
        start.setToolTipText("Start/Stop listening");
        about.setToolTipText("Version informacion");
        clear.setToolTipText("Clearing the workspace");
        fileClose.setToolTipText("Exit application");
        box1.setToolTipText("MegNemTudomMicsoda_01");
        box2.setToolTipText("MegNemTudomMicsoda_02");
        box3.setToolTipText("MegNemTudomMicsoda_03");
        box4.setToolTipText("MegNemTudomMicsoda_04");
        box5.setToolTipText("MegNemTudomMicsoda_05");
	}

	private void baseSetings() {
		startStop.setBounds(3, STATUSBAR_HEIGHT, 100, 20);								//Set places on the table
        resived.setBounds(104, STATUSBAR_HEIGHT, 120, 20);
        resevedPack.setBounds(225, STATUSBAR_HEIGHT, 50, 20);
        chanelT.setBounds(276, STATUSBAR_HEIGHT, 60, 20);
        chanel.setBounds(337, STATUSBAR_HEIGHT, 50, 20);
        loadedFileT.setBounds(388, STATUSBAR_HEIGHT, 70, 20);
        loadedFile.setBounds(459, STATUSBAR_HEIGHT, 200, 20);
        start.setBounds((screenWidth-170), 10, BUTTON_WIDTH, BUTTON_HEIGHT);			//Dynamic position sets. Status : ß
        clear.setBounds((screenWidth-85), 10, BUTTON_WIDTH, BUTTON_HEIGHT);
        backgPanel.setBounds(BACKG_PANEL_X, BACKG_PANEL_Y, BACKG_WIDTH, BACKG_HEIGHT);
        
        scrollbar.setBounds(30, screenHeight-119, screenWidth-40, 15);
        scrollbar.setValue(0);
        
        timeLine.setBounds(TIMELINE_X, TIMELINE_Y, TIMELINE_WIDTH+1000000, TIMELINE_HEIGHT);
        timeLine.setBackground(Color.white);
        																				//JCheckBox Dynamic position sets
        int height = JCHECKBOX_FIRS_SIZE;
        box1.setBounds(3, height, JCHECKBOX_SIZE, JCHECKBOX_SIZE);
		box2.setBounds(3, height+=HORISONTAL_JCB_SPEACE, JCHECKBOX_SIZE, JCHECKBOX_SIZE);
		box3.setBounds(3, height+=HORISONTAL_JCB_SPEACE, JCHECKBOX_SIZE, JCHECKBOX_SIZE);
		box4.setBounds(3, height+=HORISONTAL_JCB_SPEACE, JCHECKBOX_SIZE, JCHECKBOX_SIZE);
		box5.setBounds(3, height+=HORISONTAL_JCB_SPEACE, JCHECKBOX_SIZE, JCHECKBOX_SIZE);
		
		box1.setSelected(false);
		box2.setSelected(false);
		box3.setSelected(false);
		box4.setSelected(false);
		box5.setSelected(false);
        
	}

	private void adding() {
		snfG.add(start);											//Adding what we need on the work space
        snfG.add(clear);
        snfG.add(startStop);
        snfG.add(resived);
        snfG.add(resevedPack);
        snfG.add(chanelT);
        snfG.add(chanel);
        snfG.add(loadedFileT);
        snfG.add(loadedFile);
        snfG.add(backgPanel);
        snfG.add(scrollbar);
        
        snfG.add(box1);												//Like Checkboxes
        snfG.add(box2);
        snfG.add(box3);
        snfG.add(box4);
        snfG.add(box5);
        backgPanel.add(timeLine);
	}

	private void createActionListeners() {						
		// TODO Auto-generated method stub
		fileClose.addActionListener(new ActionListener() {			//Set Aciton Listener functions
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
                openHisoty();
            }
		});
        
        save.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent event) {
                saveHisoty();
            }
		});
        
        createOption.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent event) {
            	createOption();
            }
		});
        
        openOption.addActionListener(new ActionListener() {
            public void actionPerformed(ActionEvent event) {
            	openOption();
            }
		});
		
	}

	private void editables() {											//Edit operations to set what we can edit and what is static
	    startStop.setEditable(false);
	    resived.setEditable(false);
	    chanelT.setEditable(false);
	    resevedPack.setEditable(false);
	    resevedPack.setHorizontalAlignment(SwingConstants.TRAILING);
	    chanel.setEditable(false);
	    chanel.setHorizontalAlignment(SwingConstants.TRAILING);
	    loadedFileT.setEditable(false);
        loadedFile.setEditable(false);
	}
	
	@SuppressWarnings("unused")
	public static void main(String[] args) {
		SnifferGraph aaa = new SnifferGraph();							//A New object of that all !
    } 
}