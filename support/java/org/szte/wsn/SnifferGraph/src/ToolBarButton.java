import java.awt.*;
import javax.swing.*;

public class ToolBarButton extends JButton {
  /**
	 * 
	 */
	private static final long serialVersionUID = 11L;
private static final Insets margins =
    new Insets(0, 0, 0, 0);

  public ToolBarButton(Icon icon) {
    super(icon);
    setMargin(margins);
    setVerticalTextPosition(BOTTOM);
    setHorizontalTextPosition(CENTER);
  }

  public ToolBarButton(String imageFile) {
    this(new ImageIcon(imageFile));
  }

  public ToolBarButton(String imageFile, String text) {
    this(new ImageIcon(imageFile));
    setText(text);
  }
  
	public static void main(String[] args) {
		JFrame proba = new JFrame();
		ToolBarButton entry1 = new ToolBarButton("./src/Icon/Load-Packages.png");
		entry1.setBounds(0,0,50,50);
		entry1.setToolTipText("Load Packages");
		//entry1.addActionListener(l);
		
		ToolBarButton entry2 = new ToolBarButton("./src/Icon/Save.png");
		entry2.setBounds(50, 0,50,50);
		entry2.setToolTipText("Save");
		//entry2.addActionListener(l);
		
		ToolBarButton entry3 = new ToolBarButton("./src/Icon/Open-Configuration.png");
		entry3.setBounds(100,0,50,50);
		entry3.setToolTipText("Open Configuration");
		//entry3.addActionListener(l);
		
		ToolBarButton entry4 = new ToolBarButton("./src/Icon/Create-Configuration.png");
		entry4.setBounds(150, 0,50,50);
		entry4.setToolTipText("Create Configuration");
	//	entry4.addActionListener(l);
		
		ToolBarButton entry5 = new ToolBarButton("./src/Icon/About.png");
		entry5.setBounds(200, 0,50,50);
		entry5.setToolTipText("About");
	//	entry5.addActionListener(l);
		
		ToolBarButton entry6 = new ToolBarButton("./src/Icon/Exit.png");
		entry6.setBounds(250, 0,50,50);
		entry6.setToolTipText("Exit");
	//	entry6.addActionListener(l);
		
		proba.setSize(300, 82);
		proba.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
		proba.setLayout(null);
		proba.add(entry1);
		proba.add(entry2);
		proba.add(entry3);
		proba.add(entry4);
		proba.add(entry5);
		proba.add(entry6);
		
		//proba.add(new ToolBarButton("./src/Icon/open-icon.png"));
		proba.setVisible(true);
	}
}
