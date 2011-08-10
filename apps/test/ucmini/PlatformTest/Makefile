COMPONENT=PlatformTestC

CFLAGS += -DDIAGMSG_RADIO
CFLAGS += -DDIAGMSG_RECORDED_MSGS=100

BUILD_EXTRA_DEPS = DataMsg.java PlatformTest.class
CLEAN_EXTRA = $(BULID_EXTRA_DEPS) *.class

DataMsg.java: DataMsg.h
		mig -target=null -java-classname=DataMsg java DataMsg.h datamsg -o $@
PlatformTest.class: PlatformTest.java DataMsg.java
	javac PlatformTest.java
include $(MAKERULES)
