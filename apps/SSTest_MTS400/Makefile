COMPONENT=SSTest_MTS400_App
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
include $(MAKERULES)