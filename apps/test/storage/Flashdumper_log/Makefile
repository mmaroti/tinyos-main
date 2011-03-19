COMPONENT=FlashDumper_logC
BUILD_EXTRA_DEPS += ctrlMsg.java dataMsg.java FlashDumper.class
CLEAN_EXTRA      = ctrlMsg.java dataMsg.java *.class
ctrlMsg.java: FlashDumper_log.h
	mig -target=$(PLATFORM) -java-classname=ctrlMsg java FlashDumper_log.h ctrl_msg_t -o $@
dataMsg.java: FlashDumper_log.h
	mig -target=$(PLATFORM) -java-classname=dataMsg java FlashDumper_log.h data_msg_t -o $@
FlashDumper.class: FlashDumper.java ctrlMsg.java dataMsg.java
	javac FlashDumper.java 
include $(MAKERULES)