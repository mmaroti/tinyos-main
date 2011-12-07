README for TestC
Author/Contact: kincsesz@inf.u-szeged.hu, tbarat88@gmail.com

Description:

This application tests the GSMActiveMessageC module. In this test, the radio on the mote and the sim900 module on the ucsim900 sensorboard turn on using the ActiveMessageC and GSMActiveMessageC modules. If a command was received via the radio the application sends it to a remote server application using mobile internet connection, then waiting for the answer. When the answer was received from the remote server the application sends it via the radio. After this the application turns off the radio and the sim900 module using the ActiveMessageC and GSMActiveMessaggeC modules.
The GSMActiveMessageC module provides tree interfaces. The SplitControl interface the AMSend interface, and the Receive interface.
	SplitControl interface start command: Power on the sim900 module on the ucsim900 sensorboard, connect to a remote server application using the parameters are defined in the parameters.h (/SZTETOSDIR/chips/sim900).
	SplitControl interface stop command: Disconnect from the remote server application and power off the sim900 module on the ucsim900 sensorboard.
	AMSend interface: Same as the AMSend interface using in the case of radio communication.
	Receive interface: Same as the Receive interface using in the case of radio communication.
The parameters.h header file:
	APN: The Access Point Name of the mobile internet provider.
	USER: The user name on the mobile internet provider.
	PWD: The password on the mobile internet provider.
	UDP_TCP: The connection type (“TCP”, “UDP”).
	IP: The IP address of the remote server.
	SERVERPORT: The port opened by the remote server application.
	CONNECT_TIMEOUT: The timeout for connecting to the mobile internet provider.

Usage: 

Program the iris mote with the TestC application. Start the server.java on the remote server PC. Plug in the power source of the ucsim900 module. This is the power source of the iris mote also. The application automatically powers on the sim900 module and connects to the server application running on the remote PC. Start the DataTest.java. Enter a command. Wait for the received answer.

Tools:

server.java: Receives data from the connected client, and then mirrors it back to the client. (java server [port to open])

DataTest.java: Sends a command via the BaseStation to the mote connected to the ucsim900 sensorboard, and prints out the answer received from the remote server application (java DataTest –comm serial@comport:iris).

Known bugs/limitations:
