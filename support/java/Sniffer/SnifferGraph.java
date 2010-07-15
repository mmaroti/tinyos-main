import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Event;
import java.awt.FlowLayout;
import java.awt.Frame;
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
import javax.swing.JTextField;
import javax.swing.KeyStroke;
import javax.swing.SwingConstants;

import config.CreateConfGUI;

/**
 * @author Nemeth Gabor, Nyilas Sandor Karoly
 * This program is created to be the GUI for the Sniffer Wireless Project
 * 
 * 
 * version: 0.01b
 */

public class SnifferGraph implements DataBase{	
	
	/**
	 *Global value declarations for 
	 */
	private static final long serialVersionUID = 1L;
	
	public static ArrayList<JPanel> dataPanel = new ArrayList<JPanel>();
	
	JPanel mainPanel = new JPanel(new BorderLayout(10, 10));
	
	JPanel southPanel = new JPanel(new FlowLayout(FlowLayout.LEFT));
	JPanel northPanel = new JPanel(new FlowLayout(FlowLayout.RIGHT));
	JPanel leftPanel = new JPanel(new FlowLayout(FlowLayout.RIGHT));
	JPanel centerPanel = new JPanel(new BorderLayout(10, 10));
	
	JFrame snfG = new JFrame((String)Labels.frame_name);
	
	static JPanel backgPanel = new JPanel();
	
	JMenuBar menubar = new JMenuBar();
	XmlRead xmlRead = new XmlRead();
	
    JMenu file = new JMenu("File");
    
    JMenuItem fileClose = new JMenuItem(Labels.menu_exit);
    JMenuItem about = new JMenuItem(Labels.menu_about);
    JMenuItem open = new JMenuItem(Labels.menu_load_packages);
    JMenuItem openConfiguration = new JMenuItem(Labels.menu_open_conf);	
    JMenuItem createConfiguration = new JMenuItem(Labels.menu_creat);
    JMenuItem save = new JMenuItem(Labels.menu_save);
    
    JButton start = new JButton(Labels.button_start);
    JButton clear = new JButton(Labels.button_clear);
    
    JTextField startStop = new JTextField(Labels.txfield_pause);
    JTextField resived = new JTextField(Labels.txfield_resived);
    JTextField chanelT = new JTextField(Labels.txfield_chanelT);
    JTextField resevedPack = new JTextField("0");
    JTextField chanel = new JTextField("1L");
    JTextField loadedFileT = new JTextField(Labels.txfield_loadedFileT);
    JTextField loadedFile = new JTextField(xmlRead.filename);
    JTextField saved = new JTextField(xmlRead.filename);
    
    public static JTextField verticalText[] = new JTextField[100];
    static TimeLineDraw timeLine = new TimeLineDraw();
    
    JScrollBar scrollbar = new JScrollBar(JScrollBar.HORIZONTAL);
    
   
    
    JCheckBox box[] = new JCheckBox[xmlRead.number+1];
    
    JFileChooser fc = new JFileChooser();
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
		// TODO Auto-generated method stub
		
	}
	/**
	 * This function crates a new configuration file.
	 */
	protected void createConfiguration() {						
		System.out.println("createop");
		// TODO Auto-generated method stub
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
        
        createActionListeners();									
        
        backgPanel.setBackground(Color.white);						
        backgPanel.setBorder(BorderFactory.createEtchedBorder());	

        scrollbar.addAdjustmentListener(new MyAction());			
        scrollbar.setMaximum(screenWidth+9999-110);						
        dataPanelC();
        menuCreat();												
        editables();										
        baseSetings();										
        adding();											
        setTooltips();													
        createTimeLine();
		createBoxes();
        setMnemonics();
        
        snfG.setSize(800, 600);					
        snfG.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);		
        snfG.setExtendedState(snfG.getExtendedState() | Frame.MAXIMIZED_BOTH);	
        snfG.setVisible(true);
   }
   /**
    * At  ß version but this function can crate panels  on our workspace.
    */
   private void dataPanelC() {
		JPanel CDataP;
		int arrabb = 0;
		JTextField cell[] = new JTextField[xmlRead.number+1]; 
		for(int i=0;i<50;i++){
			CDataP = new JPanel();
			CDataP.setLayout(null);
			
			cell[0] = new JTextField(String.valueOf(Process.whatTimeIsNow()));
			cell[0].setBounds(1,arrabb,70,20);
			cell[0].setEditable(false);
			CDataP.add(cell[0]);
			
			arrabb+=HORISONTAL_JCB_SPEACE;
			for(int b=1 ;b<=xmlRead.number;b++){
				cell[b] = new JTextField(Process.dataCodeing(i, b-1));
				cell[b].setBounds(1,arrabb,70,20);
				cell[b].setEditable(false);
				CDataP.add(cell[b]);
				
				arrabb+=HORISONTAL_JCB_SPEACE;
			}
			dataPanel.add(CDataP);
			arrabb=0;
		}
	}
	/**
	 * This function creates and specify checkboxes
	 */
	private void createBoxes() {
		int height = JCHECKBOX_FIRS_SIZE;
		
		box[0]= new JCheckBox("Catch");											//Elsõ box beálítása. Az az értéket mutatja majd pontosan, hogy mikor érkezett.
		box[0].setToolTipText("Ekkor érkezett a package");
		box[0].setSelected(true);
		box[0].addActionListener(new ActionListener() {			
        public void actionPerformed(ActionEvent event) {
        		switching(event);
        	}
		});
		box[0].setBounds(3, height+=HORISONTAL_JCB_SPEACE, JCHECKBOX_SIZE_X, JCHECKBOX_SIZE_Y);
		backgPanel.add(box[0]);
		
		
		
		for(int i = 1; i <= xmlRead.number; i++){
			box[i]= new JCheckBox(xmlRead.cimkek.get(i-1));
			box[i].setToolTipText(xmlRead.title.get(i-1));
			box[i].setSelected(true);
			box[i].addActionListener(new ActionListener() {			
            public void actionPerformed(ActionEvent event) {
            		switching(event);
            	}
			});
			box[i].setBounds(3, height+=HORISONTAL_JCB_SPEACE, JCHECKBOX_SIZE_X, JCHECKBOX_SIZE_Y);
			backgPanel.add(box[i]);
		}
	}
	/**
	 * Function will used by checkboxes to know what data visible
	 * @param event
	 */
	private void switching(ActionEvent event) {
		System.out.println(event.getActionCommand()); 
		// TODO Auto-generated method stub
	}


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
        createConfiguration.setMnemonic('C');
        createConfiguration.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_C,Event.SHIFT_MASK));
        about.setMnemonic('A');
        about.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_A,Event.ALT_MASK));

	}
	/**
	 * This function draws the JtextFields under the TimeLine.
	 */
	private void createTimeLine() {											
		
		// TODO Auto-generated method stub
		for(int i = 1; i <= 100; i++){
			verticalText[i-1] = new JTextField(Process.makeTimeLineSting(i));
		}
		int k = 122;
		for(int i = 0;i < 100; i++){
			verticalText[i].setBounds(k, 80, 30, 30);
			k+=100;
			verticalText[i].setBorder(null);
			backgPanel.add(verticalText[i]);								
		}
		
	}
	/**
	 * This function specifies which objects added to the file menu.
	 */
	private void menuCreat() {
		file.add(open);
        file.add(save);
        file.add(new JSeparator());											
        
        file.add(createConfiguration);
        file.add(openConfiguration);
        file.add(new JSeparator());
        //---------------- SEPARATOR
        file.add(about);													
        file.add(new JSeparator());
        //---------------- SEPARATOR
        file.add(fileClose);												
        
        menubar.add(file);
        snfG.setJMenuBar(menubar);											
	}
	/**
	 * This function  specifying tool tips for objects.
	 * The text displays when the cursor lingers over the component.
	 */
	private void setTooltips() {
		createConfiguration.setToolTipText(Labels.tooltips_create_conf);					
        open.setToolTipText(Labels.tooltips_open);
        openConfiguration.setToolTipText(Labels.tooltips_open_conf);
        save.setToolTipText(Labels.tooltips_save);
        start.setToolTipText(Labels.tooltips_start);
        about.setToolTipText(Labels.tooltips_about);
        clear.setToolTipText(Labels.tooltips_clear);
        fileClose.setToolTipText(Labels.tooltips_fileClose);
	}
	/**
	 * This Older ß function  specified where to align exactly the objects on the workspace.
	 */
	private void baseSetings() {
		//TODO        
	}

	/**
	 * This function  adds objects to southPanel , northPanel , backgPanel , centerPanel or mainPanel.
	 * To be aligned on our workspace
	 */
	private void adding() {
		southPanel.add(startStop);
		southPanel.add(resived);
		southPanel.add(resevedPack);
		southPanel.add(chanelT);
		southPanel.add(chanel);
		southPanel.add(loadedFileT);
		southPanel.add(loadedFile);
		
		northPanel.add(start);
		northPanel.add(clear);
		timeLine.setBounds(TIMELINE_X, TIMELINE_Y, TIMELINE_WIDTH+1000000, TIMELINE_HEIGHT);		
	    timeLine.setBackground(Color.white);
		
		backgPanel.add(timeLine);
		int k = DATAPANEL_FIRST_SIZE;
		for(int i=0;i<dataPanel.size();i++){
			dataPanel.get(i).setBounds(k, DATAPANEL_Y, DATAPANEL_SIZE_X, ((xmlRead.number+3)*DATAPANEL_SIZE_Y));
			backgPanel.add(dataPanel.get(i));
			k+=DATAPANEL_SPEACE;
		}
		centerPanel.add(scrollbar, BorderLayout.SOUTH);
		centerPanel.add(backgPanel, BorderLayout.CENTER);
		
		mainPanel.add(southPanel, BorderLayout.SOUTH);
		mainPanel.add(northPanel, BorderLayout.NORTH);
		mainPanel.add(leftPanel, BorderLayout.WEST);
		mainPanel.add(centerPanel, BorderLayout.CENTER);
		
		snfG.add(mainPanel);
	}
	/**
	 * This function  specifying Action Listeners.
	 */
	private void createActionListeners() {						
		// TODO Auto-generated method stub
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
        
        createConfiguration.addActionListener(new ActionListener() {
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
		SnifferGraph main = new SnifferGraph();
		/*JPanel aa = new JPanel();
		JTextField bb = new JTextField("marsika");
		aa.setLayout(null);
		aa.add(bb);
		aa.setBounds(300,300,300,300);
		aa.setVisible(true);
		dataPanel.add(aa);
		backgPanel.add(dataPanel.get(dataPanel.size()-1));*/
    } 
}