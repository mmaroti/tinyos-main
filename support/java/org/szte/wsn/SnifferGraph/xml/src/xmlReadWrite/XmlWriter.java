package xmlReadWrite;

import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.io.Writer;
import java.util.ArrayList;
import java.util.Stack;

/**
 * Makes writing XML much much easier. 
 *
 * @author Nemeth Gabor , Nyilas Sandor Karoly
 * @version 0.1
 */
public class XmlWriter {

    private Writer writer;      // underlying writer
    private Stack<String> stack;        // of xml entity names
    private StringBuffer attrs; // current attribute string
    private boolean empty;      // is the current node empty
    private boolean closed;     // is the current node closed...

    /**
     * Create an XmlWriter on top of an existing java.io.Writer.
     */
    public XmlWriter(Writer writer) {
        this.writer = writer;
        this.closed = true;
        this.stack = new Stack<String>();
    }

    /**
     * Begin to output an entity. 
     *
     * @param String name of entity.
     * @throws Exception 
     */
    public XmlWriter writeEntity(String name) throws Exception {
        try {
            closeOpeningTag();
            this.closed = false;
            this.writer.write("<");
            this.writer.write(name);
            stack.add(name);
            this.empty = true;
            return this;
        } catch (IOException ioe) {
            throw new Exception(ioe);
        }
    }

    // close off the opening tag
    private void closeOpeningTag() throws IOException {
        if (!this.closed) {
            writeAttributes();
            this.closed = true;
            this.writer.write(">");
        }
    }

    // write out all current attributes
    private void writeAttributes() throws IOException {
        if (this.attrs != null) {
            this.writer.write(this.attrs.toString());
            this.attrs.setLength(0);
            this.empty = false;
        }
    }

    /**
     * Write an attribute out for the current entity. 
     * Any xml characters in the value are escaped.
     * Currently it does not actually throw the exception, but 
     * the api is set that way for future changes.
     *
     * @param String name of attribute.
     * @param String value of attribute.
     * @throws Exception 
     */
    public XmlWriter writeAttribute(String attr, String value) throws Exception {

        if (this.attrs == null) {
            this.attrs = new StringBuffer();
        }
        this.attrs.append(" ");
        this.attrs.append(attr);
        this.attrs.append("=\"");
        this.attrs.append(escapeXml(value));
        this.attrs.append("\"");
        return this;
    }

    /**
     * End the current entity. This will throw an exception 
     * if it is called when there is not a currently open 
     * entity.
     * @throws Exception 
     */
    public XmlWriter endEntity() throws Exception {
        try {
            if(this.stack.empty()) {
                throw new Exception("Called endEntity too many times. ");
            }
            String name = (String)this.stack.pop();
            if (name != null) {
                if (this.empty) {
                    writeAttributes();
                    this.writer.write("/>");
                } else {
                    this.writer.write("</");
                    this.writer.write(name);
                    this.writer.write(">");
                }
                this.empty = false;
            }
            return this;
        } catch (IOException ioe) {
            throw new Exception(ioe);
        }
    }

    /**
     * Close this writer. It does not close the underlying 
     * writer, but does throw an exception if there are 
     * as yet unclosed tags.
     * @throws IOException 
     */
    public void close() throws WritingException, IOException {
        if(!this.stack.empty()) {
            throw new IOException("Tags are not all closed. "+
                "Possibly, "+this.stack.pop()+" is unclosed. ");
        }
    }

    /**
     * Output body text. Any xml characters are escaped. 
     * @throws IOException 
     */
    public XmlWriter writeText(String text) throws WritingException, IOException {
        try {
            closeOpeningTag();
            this.empty = false;
            this.writer.write(escapeXml(text));
            return this;
        } catch (IOException ioe) {
            throw new IOException(ioe);
        }
    }

    // Static functions lifted from generationjava helper classes
    // to make the jar smaller.
    
    // from XmlW
    static public String escapeXml(String str) {
        str = replaceString(str,"&","&amp;");
        str = replaceString(str,"<","&lt;");
        str = replaceString(str,">","&gt;");
        str = replaceString(str,"\"","&quot;");
        str = replaceString(str,"'","&apos;");
        return str;
    }  

    // from StringW
    static public String replaceString(String text, String repl, String with) {
        return replaceString(text, repl, with, -1);
    }  
    /**
     * Replace a string with another string inside a larger string, for
     * the first n values of the search string.
     *
     * @param text String to do search and replace in
     * @param repl String to search for
     * @param with String to replace with
     * @param n    int    values to replace
     *
     * @return String with n values replacEd
     */
    static public String replaceString(String text, String repl, String with, int max) {
        if(text == null) {
            return null;
        }
 
        StringBuffer buffer = new StringBuffer(text.length());
        int start = 0;
        int end = 0;
        while( (end = text.indexOf(repl, start)) != -1 ) {
            buffer.append(text.substring(start, end)).append(with);
            start = end + repl.length();
 
            if(--max == 0) {
                break;
            }
        }
        buffer.append(text.substring(start));
 
        return buffer.toString();
    }              
    

    static public void test2(ArrayList<String> inputs) throws Exception {
    	Writer writer = new java.io.StringWriter();
    	int help=0;
    	for(int b=0;b<inputs.size()-1;b++){
    		System.err.println(inputs.get(b));
    	}
    	XmlWriter xmlwriter = new XmlWriter(writer);;
        xmlwriter.writeEntity("packet");
        for(int i=0;i<(inputs.size()/4);i++){
        	xmlwriter.writeEntity("entity");
        	xmlwriter.writeAttribute("name", "Ez a(z): "+(i+1)+".");

    
       
        	xmlwriter.writeEntity("startByte");
        	xmlwriter.writeText((inputs.get(help)));
        	xmlwriter.endEntity();
       
       		xmlwriter.writeEntity("stopByte");
       		xmlwriter.writeText((inputs.get(help+1)));
       		xmlwriter.endEntity();
       
     
        	xmlwriter.writeEntity("type");
        	xmlwriter.writeText((inputs.get(help+2)));
        	xmlwriter.endEntity();
        
    
        	xmlwriter.writeEntity("cimke");
        	xmlwriter.writeText((inputs.get(help+3)));
        	xmlwriter.endEntity();
        
        	xmlwriter.endEntity();
        	help+=4;
        }
        
        xmlwriter.endEntity();
        xmlwriter.close();
       help=0;
        
        
        System.err.println(writer.toString());
        try{
        FileWriter fstream = new FileWriter("./xml/SavedConfout.xml");
        BufferedWriter out = new BufferedWriter(fstream);
       
		out.write(header());
    out.write(writer.toString());

    //Close the output stream
    out.close();
    }catch (Exception e){
      System.err.println("Error: " + e.getMessage());
    }
    }

	private static String header() {
		
		return "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>";
	}
}
