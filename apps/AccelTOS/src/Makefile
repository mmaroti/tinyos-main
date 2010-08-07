COMPONENT = RadioHandlerC
#CFLAGS += -I$(SZTETOSDIR)/lib/shimmer
CFLAGS += -DTOSH_DATA_LENGTH=96
CFLAGS += -DCC2420_DEF_CHANNEL=11
CFLAGS += -I$(TOSDIR)/lib/diagmsg
CFLAGS += -DDIAGMSG_RADIO
CFLAGS += -DDIAGMSG_RETRY_COUNT=1
#CFLAGS += -DDIAGMSG_BASE_STATION=0
CFLAGS += -DASSERT_LEDON
#CFLAGS += -DSHIMMER_ADC_TEST
#CFLAGS += -DSHIMMER_MEMORY_FILE

BUILD_EXTRA_DEPS = CtrlMsg.class ReportMsg.class
CLEAN_EXTRA = *.class CtrlMsg.java ReportMsg.java

CtrlMsg.class: CtrlMsg.java
	javac CtrlMsg.java

CtrlMsg.java:
	mig java -target=shimmer2 -java-classname=CtrlMsg CtrlMsg.h CtrlMsg -o $@

#DataMsg.class: DataMsg.java
#	javac DataMsg.java

#DataMsg.java:
#	mig java -target=shimmer2 -java-classname=DataMsg DataMsg.h DataMsg -o $@

ReportMsg.class: ReportMsg.java
	javac ReportMsg.java

ReportMsg.java:
	mig java -target=shimmer2 -java-classname=ReportMsg ReportMsg.h ReportMsg -o $@ 

include $(MAKERULES) 
