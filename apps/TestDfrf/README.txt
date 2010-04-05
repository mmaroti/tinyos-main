TestDfrf

This application tests the Directed FloodRouting Framework. A dedicated beacon 
mote (programmed with node ID 1) is periodically broadcasting a message with 
incrementing counter value. The period is set to .75 sec. The rest of the motes 
are setting their LEDs to the lowest 3 bits of the received counter value and 
are relaying the messages. A mote programmed with the BaseStationDfrf 
application bridges the messages to the PC, where the message content can be 
displayed with MessageCenter/MessageTable.


Usage:

- Program two or more motes with the TestDfrf application. Make sure that one of 
the nodes has node ID 1 -- this will be the beacon mote.

- Program a mote with the BaseStationDfrf application and leave it connected to 
the PC.

- Start SerialForwarder on the PC (e.g. "java net.tinyos.sf.SerialForwarder -comm 
serial@com5:telos" if using a telos mote on com5)

- Start MessageCenter ( java net.tinyos.mcenter.MessageCenter )

- To specify a data source, click the "Configuration" tab on the SerialConnector 
window. Type the name of the new connection to the editable combo box (e.g. 
"local serialforwarder 1"). Select the "Remote Server" radio button,  type 
"127.0.0.1" in the address box and "9002" in the port box. Click the save 
button.

- Click the "Configuration" tab on the SerialConnector window. Click the start 
button next to the name of the connection ("local serialforwarder 1").

- Start the MessageTable applet by typing "net.tinyos.mcenter.MessageTable" in 
the App Loader window, then clicking the Load App button.

- To create a new configuration, type "TestDfrf" in the editable combo box, 
click on the message format tab, set message type to 130, click the "big endian" 
checkbox and type the following message format specification in the textbox:

	uint8_t appid
	uint16_t location
	uint16_t srcid
	uint8_t counter

Click Save and Reset, then switch back to the RcvdData tab. Beacon messages 
should be arriving.
