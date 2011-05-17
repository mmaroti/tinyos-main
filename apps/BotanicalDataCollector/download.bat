@echo off
set /p COM=Please enter serial port number: 
java -jar download.jar serial@com%COM%:iris