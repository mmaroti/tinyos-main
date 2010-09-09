COMPONENT=MicReadStreamAppC
CFLAGS += -DASSERT_NONE
#CFLAGS += -DEIGHT_BIT_DATA
CFLAGS += -DTOSH_DATA_LENGTH=114
CFLAGS += -DRF230_BACKOFF_MIN=120
CFLAGS += -DRF230_BACKOFF_INIT=200
CFLAGS += -DRF230_BACKOFF_CONG=200
CFLAGS += -DCC2420_DEF_CHANNEL=15
CFLAGS += -DRF230_DEF_CHANNEL=15
CFLAGS += -I$(SZTETOSDIR)/lib/Atm128FastAdc
CFLAGS += -I$(SZTETOSDIR)/lib/Mts300
SENSORBOARD=mts300

BUILD_EXTRA_DEPS = MicMsg.java Capture.class
CLEAN_EXTRA = $(BUILD_EXTRA_DEPS) MicMsg.java *.class

MicMsg.java: MicReadStream.h
	mig -target=null -java-classname=MicMsg java MicReadStream.h datamsg -o $@
Capture.class: MicMsg.java Capture.java
	javac Capture.java
include $(MAKERULES)
