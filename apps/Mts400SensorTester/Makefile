COMPONENT=Mts400TesterC
CFLAGS += -I$(TOSDIR)/sensorboards/mts400
CFLAGS += -I$(TOSDIR)/chips/sht11

BUILD_EXTRA_DEPS = DataMsg.java
CLEAN_EXTRA = $(BULID_EXTRA_DEPS)

DataMsg.java: DataMsg.h
		mig -target=null -java-classname=DataMsg java DataMsg.h datamsg -o $@

include $(MAKERULES)
