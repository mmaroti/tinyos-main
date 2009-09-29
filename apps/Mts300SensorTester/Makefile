COMPONENT=Mts300SensorTesterAppC
PFLAGS = -I$(TOSDIR)/sensorboards/mts300 -I$(TOSDIR)/lib/MicReadStream
CFLAGS += -DSAMP_TIME=1000

BUILD_EXTRA_DEPS = DataMsg.java ControlMsg.java
CLEAN_EXTRA = $(BUILD_EXTRA_DEPS) DataMsg.java ControlMsg.java

DataMsg.java: Mts300SensorMsg.h
	mig -target=null -java-classname=DataMsg java Mts300SensorMsg.h datamsg -o $@

ControlMsg.java: Mts300SensorMsg.h
	mig -target=null -java-classname=ControlMsg java Mts300SensorMsg.h controlmsg -o $@
include $(MAKERULES)
