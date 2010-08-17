COMPONENT=ApplicationC
BUILD_EXTRA_DEPS= GetMsg.java DataMsg.java CtrlMsg.java CommandMsg.java
CFLAGS += -DMTS300CB
SENSORBOARD=mts300
CFLAGS += -I$(SZTETOSDIR)/lib/StreamStorage
CFLAGS += -I$(SZTETOSDIR)/lib/StorageFrame

GetMsg.java: StreamUploader.h
	mig java -target=$(PLATFORM) -java-classname=GetMsg StreamUploader.h get_msg -o $@

DataMsg.java: StreamUploader.h
	mig java -target=$(PLATFORM) -java-classname=DataMsg StreamUploader.h data_msg -o $@

CtrlMsg.java: StreamUploader.h
	mig java -target=$(PLATFORM) -java-classname=CtrlMsg StreamUploader.h ctrl_msg -o $@

CommandMsg.java: StreamUploader.h
	mig java -target=$(PLATFORM) -java-classname=CommandMsg StreamUploader.h command_msg -o $@

include $(MAKERULES)