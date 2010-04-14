COMPONENT=SSTest_MTS400_App
BUILD_EXTRA_DEPS += ctrlMsg.java dataMsg.java ctrltsMsg.java BinReader.class StreamDownloader.class StreamEraser.class
CLEAN_EXTRA      = ctrlMsg.java dataMsg.java ctrltsMsg.java *.class
CFLAGS += -I$(TOSDIR)/chips/sht11
CFLAGS += -I$(SZTETOSDIR)/lib/Mts400
CFLAGS += -I$(SZTETOSDIR)/lib/FastSerial
CFLAGS += -I$(SZTETOSDIR)/lib/StreamStorage
CFLAGS += -I$(SZTETOSDIR)/lib/StorageFrame
CFLAGS += -I$(SZTETOSDIR)/lib/StreamUploader
#sampling time in ms, default: 60000U
#CFLAGS += -DSAMP_T=60000U
#short waiting time for the radio in ms, default: 1000U
#CFLAGS += -DRADIO_SHORT=1000U
#long waiting time for the radio in s, default: 60U
#CFLAGS += -DRADIO_LONG=60U
#disable radio
#CFLAGS += -DRADIO_DISABLE
#disable measuring
#CFLAGS += -DMEAS_DISABLE
#disable flash writing
#CFLAGS += -DFLASHWRITE_DISABLE
ctrlMsg.java: $(SZTETOSDIR)/lib/StreamUploader/StreamUploader.h
	mig -target=$(PLATFORM) -java-classname=ctrlMsg java $(SZTETOSDIR)/lib/StreamUploader/StreamUploader.h ctrl_msg_t -o $@
ctrltsMsg.java: $(SZTETOSDIR)/lib/StreamUploader/StreamUploader.h
	mig -target=$(PLATFORM) -java-classname=ctrltsMsg java $(SZTETOSDIR)/lib/StreamUploader/StreamUploader.h ctrlts_msg_t -o $@
dataMsg.java: $(SZTETOSDIR)/lib/StreamUploader/StreamUploader.h
	mig -target=$(PLATFORM) -java-classname=dataMsg java $(SZTETOSDIR)/lib/StreamUploader/StreamUploader.h data_msg_t -o $@
BinReader.class: BinReader.java dataFile.java
	javac BinReader.java 
StreamDownloader.class: StreamDownloader.java dataFile.java ctrlMsg.java dataMsg.java ctrltsMsg.java
	javac StreamDownloader.java 
StreamEraser.class: StreamEraser.java ctrlMsg.java dataMsg.java ctrltsMsg.java
	javac StreamEraser.java 
include $(MAKERULES)