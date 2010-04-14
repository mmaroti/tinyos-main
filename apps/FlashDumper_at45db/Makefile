COMPONENT=FlashDumper_at45dbC
BUILD_EXTRA_DEPS += ctrlMsg.java dataMsg.java FlashDumper.class
CLEAN_EXTRA      = ctrlMsg.java dataMsg.java *.class
ctrlMsg.java: FlashDumper_at45db.h
	mig -target=$(PLATFORM) -java-classname=ctrlMsg java FlashDumper_at45db.h ctrl_msg_t -o $@
dataMsg.java: FlashDumper_at45db.h
	mig -target=$(PLATFORM) -java-classname=dataMsg java FlashDumper_at45db.h data_msg_t -o $@
FlashDumper.class: FlashDumper.java ctrlMsg.java dataMsg.java
	javac FlashDumper.java 
include $(MAKERULES)