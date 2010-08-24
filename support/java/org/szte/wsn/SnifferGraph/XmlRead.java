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
* This class cares with the file configuration.xml file reading 
* version: 0.01b
* */

package org.szte.wsn.SnifferGraph;

import java.io.File;
import java.io.IOException;
import java.util.ArrayList;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

public class XmlRead {
	
	public int number;
	public ArrayList<Integer> tagok = new ArrayList<Integer>();
	public ArrayList<String> cimkek = new ArrayList<String>();
	public ArrayList<String> title = new ArrayList<String>();
	public String filename;
	
	
	public XmlRead(){
		try{
		DocumentBuilderFactory docBuilderFactory = DocumentBuilderFactory.newInstance();
        DocumentBuilder docBuilder = docBuilderFactory.newDocumentBuilder();
        File file = new File(Labels.path);
        filename = file.getName();
        Document doc = docBuilder.parse (file);
        
        NodeList listOfenity = doc.getElementsByTagName("entity");
        number = listOfenity.getLength();
        
        for(int i=0; i<listOfenity.getLength() ; i++){
        	Node firstValue = listOfenity.item(i);
        	
            if(firstValue.getNodeType() == Node.ELEMENT_NODE){


                Element valuesOf = (Element)firstValue;

                //-------
                NodeList firstNameList = valuesOf.getElementsByTagName("startByte");
                Element firstNameElement = (Element)firstNameList.item(0);

                NodeList textFNList = firstNameElement.getChildNodes();
                tagok.add(Integer.parseInt(((Node)textFNList.item(0)).getNodeValue().trim()));

                //-------
                NodeList lastNameList = valuesOf.getElementsByTagName("stopByte");
                Element lastNameElement = (Element)lastNameList.item(0);
                
                NodeList textLNList = lastNameElement.getChildNodes();
                tagok.add(Integer.parseInt(((Node)textLNList.item(0)).getNodeValue().trim()));

                //----
                NodeList ageList = valuesOf.getElementsByTagName("type");
                Element ageElement = (Element)ageList.item(0);

                NodeList textAgeList = ageElement.getChildNodes();
                title.add((String)((Node)textAgeList.item(0)).getNodeValue().trim());

                //------
                NodeList cimkeList = valuesOf.getElementsByTagName("cimke");
                Element cimkeElement = (Element)cimkeList.item(0);

                NodeList textCimkeList = cimkeElement.getChildNodes();
                cimkek.add((String)((Node)textCimkeList.item(0)).getNodeValue().trim());


            }//end of if clause
        }
        
        
		}catch (IOException e){
			
		} catch (ParserConfigurationException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		} catch (SAXException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		
	}


}
