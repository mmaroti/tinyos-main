COMPONENT=SenderC
BUILD_EXTRA_DEPS = DataMsg.java Hyperterminal.class
CLEAN_EXTRA = $(BULID_EXTRA_DEPS) *.class
DataMsg.java: DataMsg.h
	mig -target=null -java-classname=DataMsg java DataMsg.h datamsg -o $@
Hyperterminal.class: Hyperterminal.java DataMsg.java
	javac Hyperterminal.java
include $(MAKERULES)

