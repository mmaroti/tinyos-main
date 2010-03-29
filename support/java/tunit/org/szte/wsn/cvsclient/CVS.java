/*
* Copyright (c) 2009, University of Szeged
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
* Author:Andras Biro
*/

package org.szte.wsn.cvsclient;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.IOException;
import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import org.apache.log4j.Logger;
import org.netbeans.lib.cvsclient.CVSRoot;
import org.netbeans.lib.cvsclient.Client;
import org.netbeans.lib.cvsclient.admin.StandardAdminHandler;
import org.netbeans.lib.cvsclient.command.CommandAbortedException;
import org.netbeans.lib.cvsclient.command.CommandException;
import org.netbeans.lib.cvsclient.command.GlobalOptions;
import org.netbeans.lib.cvsclient.command.checkout.CheckoutCommand;
import org.netbeans.lib.cvsclient.command.update.UpdateCommand;
import org.netbeans.lib.cvsclient.connection.AuthenticationException;
import org.netbeans.lib.cvsclient.connection.PServerConnection;
import org.netbeans.lib.cvsclient.event.CVSAdapter;
import org.netbeans.lib.cvsclient.event.MessageEvent;


public class CVS {
	String root="",userdir="";
	public Calendar date=null;
	public Calendar enddate=null;
	public String module="";
	public Boolean isdateset=false;
	
	private int step=1;
	/** Logging */
	private static Logger log = Logger.getLogger(CVS.class);
	
	public CVS(File cvsfile){
		log.info("Found .cvs in "+cvsfile.getAbsolutePath());
		if(parsefile(cvsfile)){
			userdir=cvsfile.getParent();
			if(isvalidCVSdir(cvsfile))
				if(date==null)
					update(userdir+File.pathSeparator+module);
				else{
					log.warn("Local files are probably newer");
					//TODO deleting by date would be much nicer
					deldir(new File(cvsfile.getParentFile().getAbsolutePath()+"/"+module),true);
					checkout();
				}
			else
				checkout();
			}
	}
	

	private boolean parsefile(File cvsfile) {
		log = Logger.getLogger(getClass());
		try {
			BufferedReader in = new BufferedReader(new FileReader(cvsfile));
			String line;
			while ((line = in.readLine()) != null) {
				line=line.toLowerCase();
				SimpleDateFormat parse=new SimpleDateFormat("yyyy-MM-dd");
				if(line.startsWith("@root")){
					root=line.replace("@root", "").trim();
				}
				if(line.startsWith("@module")){
					module=line.replace("@module", "").trim();
				}
				if(line.startsWith("@date")){
					
					try {
						parse.parse(line.replace("@date", "").trim());
					} catch (ParseException e) {
						log.error("Can't parse date: "+line+" Supported date format: yyyy-MM-dd");
					}
					date=parse.getCalendar();
					isdateset=true;

				}
				if(line.startsWith("@enddate")){
					try {
						parse.parse(line.replace("@enddate", "").trim());
					} catch (ParseException e) {
						log.error("Can't parse date: "+line+" Supported date format: MM/dd/yyyy");
					}
					enddate=parse.getCalendar();
				}
				if(line.startsWith("@step")){
					step=Integer.valueOf(line.replace("@step", "").trim());
				}
			}
			if(date==null){
				date = Calendar.getInstance();
			}
			if(enddate==null){
				enddate=(Calendar) date.clone();
				enddate.add(Calendar.DAY_OF_MONTH, -1);
			}
			in.close();
			if(root!=""&&module!="")
				return true;
			else
				return false;
		} catch (FileNotFoundException e) {
			log.error("Can't open file: "+cvsfile.getAbsolutePath());
			return false;
		} catch (IOException e) {
			log.error("Can't read file: "+cvsfile.getAbsolutePath());
			return false;
		}
	}

	private void checkout() {
		log.info("Starting CVS checkout");
		CVSRoot cvsroot;
		try{
			cvsroot= CVSRoot.parse(root);
		} catch(IllegalArgumentException e){
			log.error("Can't parse CVS root: "+root);
			return;
		}
		if(cvsroot.getMethod().equals("pserver")){
			GlobalOptions globalOptions = new GlobalOptions();
		    globalOptions.setCVSRoot(root);
		    log.info("CVS root: "+root);
			PServerConnection c = new PServerConnection(cvsroot);
			CheckoutCommand command = new CheckoutCommand();
		    try {
				c.open();
				Client client = new Client(c, new StandardAdminHandler());
				client.setLocalPath(userdir);
				client.getEventManager().addCVSListener(new BasicListener());
				command.setCheckoutByDate(date.get(Calendar.YEAR)+"-"+
											  (date.get(Calendar.MONTH)+1)+"-"+
											  date.get(Calendar.DAY_OF_MONTH)+" "+
											  date.get(Calendar.HOUR_OF_DAY)+":"+
											  date.get(Calendar.MINUTE)
											  );	
				command.setModule(module);
				command.setBuilder(null);
				command.setRecursive(true);
				//command.setPruneDirectories(true);
				log.info("CVS command: "+command.getCVSCommand());
				client.executeCommand(command, globalOptions);
				c.close();
			} catch (CommandAbortedException e) {
				log.error("CVS command was aborted: "+command.getCVSCommand());
			} catch (AuthenticationException e) {
				log.error("CVS authentication error: "+command.getCVSCommand());
			} catch (IOException e) {
				log.error("Can't close CVS connection");
			} catch (CommandException e) {
				log.error("Can't execute CVS command: "+command.getCVSCommand());
				e.printStackTrace();
			}        

		} else if(cvsroot.getMethod().equals("server")){
			log.error("server methode not supported (only pserver)");
			//TODO server connectiontype
		}
		
	}

	public void update(String dir) {
		log.info("Starting CVS update");
		CVSRoot cvsroot;
		try{
			cvsroot= CVSRoot.parse(root);
		} catch(IllegalArgumentException e){
			log.error("Can't parse CVS root: "+root);
			return;
		}
		if(cvsroot.getMethod().equals("pserver")){
			GlobalOptions globalOptions = new GlobalOptions();
		    globalOptions.setCVSRoot(root);
		    log.info("CVS root: "+root);
			PServerConnection c = new PServerConnection(cvsroot);
			UpdateCommand command = new UpdateCommand();
		    try {
				c.open();
				Client client = new Client(c, new StandardAdminHandler());
				client.setLocalPath(dir);
				client.getEventManager().addCVSListener(new BasicListener());
				command.setBuilder(null);
				command.setRecursive(true);
				//command.setPruneDirectories(true);
				//command.setBuildDirectories(true);
				command.setUpdateByDate(date.get(Calendar.YEAR)+"-"+
											  (date.get(Calendar.MONTH)+1)+"-"+
											  date.get(Calendar.DAY_OF_MONTH)+" "+
											  date.get(Calendar.HOUR_OF_DAY)+":"+
											  date.get(Calendar.MINUTE)											  
											  );
				log.info("CVS command: "+command.getCVSCommand());
				client.executeCommand(command, globalOptions);
				c.close();
			} catch (CommandAbortedException e) {
				log.error("CVS command was aborted: "+command.getCVSCommand());
			} catch (AuthenticationException e) {
				log.error("Can't connect to CVS server: "+root);
			} catch (IOException e) {
				log.error("Can't close CVS connection");
			} catch (CommandException e) {
				log.error("Can't execute CVS command: "+command.getCVSCommand());;
			}        

		} else if(cvsroot.getMethod().equals("server")){
			log.error("server methode not supported (only pserver)");
			//TODO server connectiontype
		}
	}

	private boolean isvalidCVSdir(File cvsfile){
		String oldroot = null;
	    BufferedReader r = null;
	    File rootFile = null;
	    try
	    {
	    	rootFile = new File(cvsfile.getParentFile().getAbsolutePath()+"/"+module+"/CVS/Root");
	        if (rootFile.exists())
	        {
	            r = new BufferedReader(new FileReader(rootFile));
	            oldroot = (r.readLine()).trim();         
	        } else
	        	return false;	//CVS dir doesn't exist -> checkout
	    }
	    catch (IOException e)
	    {
	        log.error("Can't read file: "+rootFile.getAbsolutePath());
	        deldir(rootFile.getParentFile(),true);
	    }
	    finally
	    {
		      if (r != null){
		    	try {
					r.close();
				} catch (IOException e) {
					log.error("Can't close file: "+rootFile.getAbsolutePath());
				}
		    	if(oldroot.equalsIgnoreCase(root)){
		    		return true;	//CVS dir's root is the same as the config
		    	} else {			//CVSroot has changed since last update
		    		log.warn("CVSRoot changed since last update");
			        deldir(rootFile.getParentFile(),true);
			        return false;
		    	}
		      }
	       
	    }
	    return false;	//shouldn't reach this point
	}
	
	private void deldir(File cvsdir,boolean needlog) {
		if(needlog)
			log.warn("Deleting directory: "+cvsdir.getAbsolutePath());
		File[] cvsfiles=cvsdir.listFiles();
		for(int i=0;i<cvsfiles.length;i++){
			if(cvsfiles[i].isDirectory())
				deldir(cvsfiles[i],false);
			else
				cvsfiles[i].delete();
		}
		cvsdir.delete();		
	}

	public Boolean updatecvs(String dir){
		date.add(Calendar.DAY_OF_MONTH, step);
    	if(date.after(enddate))
    		return false;
    	else{
    		update(dir);
    		return true;
    	}
	}
	
	
	public class BasicListener extends CVSAdapter
	{
	    /**
	     * Stores a tagged line
	     */
//	    private final StringBuffer taggedLine = new StringBuffer();

	    /**
	     * Called when the server wants to send a message to be displayed to
	     * the user. The message is only for information purposes and clients
	     * can choose to ignore these messages if they wish.
	     * @param e the event
	     */
	    public void messageSent(MessageEvent e)
	    {
	        String line = e.getMessage();
	        
	        if (e.isTagged())
	        {	//log for every single file
//	            String message = MessageEvent.parseTaggedMessage(taggedLine, line);
//		    // if we get back a non-null line, we have something
//		    // to output. Otherwise, there is more to come and we
//		    // should do nothing yet.
//	            if ((message != null)&&(message!=""))
//	            {
//	                log.info(message);
//	            }
	        }
	        else if(line!="")
	        {
	            log.info(line);
	        }
	    }
	}
}