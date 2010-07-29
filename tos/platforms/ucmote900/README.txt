
PROGRAMMING:

The org.szte.wsn.zigbit.Programmer java packet is used to program the 
zigbit900 mote with its bootloader. The build scripts automatically 
executes this program, but you have to make sure that java is properly 
installed and both this file and the tinyos.jar is in your CLASSPATH. 
To make sure that this program is properly installed, try executing it 
by
	java org.szte.wsn.zigbit

You can install your tinyos program with

	make ucmote900 install zbp,com25

line, where zbp stands for the zigbit programmer, and com25 is the 
communication port. You can also set the communication port in the 
ZBP_PORT environment variable.

This script tries to reset the mote by sending a special packet over 
the serial line. If the reset does not work, then you can reset the 
mote manually, so the programming will continue. The script will use 
several baud rates to communicate with the mote, but you can set this
in the ZBP_BAUDRATE encironment variableif you use a nonstandard baud 
rate.

This software reset reqiures the SerialActiveMessageC to be started, 
otherwise the reset command is not received. The serial line is not 
started automatically, since that consumes more power (the MCU cannot 
enter power save mode). However, for testing and development purposes 
you can define the ZBP_AUTOSTART variable to get the serial line 
statred automatically:

	CFLAGS += -DZBP_AUTOSTART

You can disable the serial reset functionality alltogether by adding
this tou your makefile:

	CFLAGS += -DZBP_DISABLE_RESET

