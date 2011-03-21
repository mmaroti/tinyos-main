COMPONENT=Mts300SensorTesterAppC
CFLAGS += -DSAMP_TIME=1000
CFLAGS += -DMTS300CB
SENSORBOARD=mts300
BUILD_EXTRA_DEPS = DataMsg.java ControlMsg.java Mts300SensorTester.class
CLEAN_EXTRA = $(BULID_EXTRA_DEPS) *.class

DataMsg.java: Mts300SensorMsg.h
	mig -target=null -java-classname=DataMsg java Mts300SensorMsg.h datamsg -o $@
ControlMsg.java: Mts300SensorMsg.h
	mig -target=null -java-classname=ControlMsg java Mts300SensorMsg.h controlmsg -o $@
	
Mts300SensorTester.class: Mts300SensorTester.java DataMsg.java ControlMsg.java
	javac Mts300SensorTester.java

include $(MAKERULES) 
