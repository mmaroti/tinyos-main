COMPONENT=MicReadStreamAppC
CFLAGS += -DASSERT_NONE
%CFLAGS += -DEIGHT_BIT_DATA
CFLAGS += -DTOSH_DATA_LENGTH=114
CFLAGS += -I$(SZTETOSDIR)/lib/Atm128FastAdc
CFLAGS += -I$(SZTETOSDIR)/lib/Mts300
CFLAGS += -I$(TOSDIR)/sensorboards/mts300
BUILD_EXTRA_DEPS = MicMsg.java
CLEAN_EXTRA = $(BUILD_EXTRA_DEPS) MicMsg.java

MicMsg.java: MicReadStream.h
	mig -target=null -java-classname=MicMsg java MicReadStream.h datamsg -o $@
include $(MAKERULES)
