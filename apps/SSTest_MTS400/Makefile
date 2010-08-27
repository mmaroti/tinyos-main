COMPONENT=SSTest_MTS400_App
BUILD_EXTRA_DEPS += ctrlMsg.java dataMsg.java ctrltsMsg.java BinReader.class StreamDownloader.class StreamEraser.class
CLEAN_EXTRA      = ctrlMsg.java dataMsg.java ctrltsMsg.java *.class
SENSORBOARD = mts400
# CFLAGS += -I$(TOSDIR)/chips/sht11
# CFLAGS += -I$(SZTETOSDIR)/lib/Mts400
CFLAGS += -I$(SZTETOSDIR)/lib/FastSerial
CFLAGS += -I$(SZTETOSDIR)/lib/StreamStorage
CFLAGS += -I$(SZTETOSDIR)/lib/StorageFrame
CFLAGS += -I$(SZTETOSDIR)/lib/StreamUploaderSH
#sampling time in ms, default: 61140U (1s=1024ms, so it's 1 minute)
#CFLAGS += -DSAMP_T=61440U
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
ctrlMsg.java: $(SZTETOSDIR)/lib/StreamUploaderSH/StreamUploader.h
	mig -target=$(PLATFORM) $(CFLAGS)  -java-classname=ctrlMsg java $(SZTETOSDIR)/lib/StreamUploaderSH/StreamUploader.h ctrl_msg_t -o $@
ctrltsMsg.java: $(SZTETOSDIR)/lib/StreamUploaderSH/StreamUploader.h
	mig -target=$(PLATFORM) $(CFLAGS)  -java-classname=ctrltsMsg java $(SZTETOSDIR)/lib/StreamUploaderSH/StreamUploader.h ctrlts_msg_t -o $@
dataMsg.java: $(SZTETOSDIR)/lib/StreamUploaderSH/StreamUploader.h
	mig -target=$(PLATFORM) $(CFLAGS)  -java-classname=dataMsg java $(SZTETOSDIR)/lib/StreamUploaderSH/StreamUploader.h data_msg_t -o $@
BinReader.class: BinReader.java dataFile.java
	javac -cp $(CLASSPATH):$(SZTETOSROOT)/support/java/DataProcess BinReader.java 
StreamDownloader.class: StreamDownloader.java ctrlMsg.java dataMsg.java ctrltsMsg.java
	javac -cp $(CLASSPATH):$(SZTETOSROOT)/support/java/DataProcess StreamDownloader.java 
StreamEraser.class: StreamEraser.java ctrlMsg.java dataMsg.java ctrltsMsg.java
	javac StreamEraser.java 
include $(MAKERULES)