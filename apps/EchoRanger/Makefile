COMPONENT=EchoRangerAppC
BUILD_EXTRA_DEPS += ERParser.class
CFLAGS += -I$(SZTETOSDIR)/sensorboards/Mts300
CFLAGS += -I$(SZTETOSDIR)/lib/Atm128FastAdc
CFLAGS += -I$(SZTETOSDIR)/lib/StreamStorage
CFLAGS += -I$(SZTETOSDIR)/lib/StorageFrame
CFLAGS += -I$(SZTETOSDIR)/lib/StreamUploader
SENSORBOARD=mts300
CFLAGS += -DMTS300CB

ERParser.class: ERParser.java
	javac -cp $(CLASSPATH):$(SZTETOSROOT)/support/java/DataProcess ERParser.java 

include $(MAKERULES)
