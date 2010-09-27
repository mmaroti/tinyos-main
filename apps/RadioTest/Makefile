COMPONENT=RadioTestAppC

CFLAGS += -DCC2420_DEF_CHANNEL=11
CFLAGS += -DRF230_DEF_CHANNEL=11

CFLAGS += -DLOW_POWER_LISTENING
CFLAGS += -DTOSH_DATA_LENGTH=50

CFLAGS += -I$(SZTETOSDIR)/lib/CodeProfile

include Makemigrules
include $(MAKERULES)
include $(SZTETOSROOT)/support/make/Makeplatformopts

USE_CC2420X_EXPERIMENTAL=yes
ifdef RADIO_CC2420
	ifeq ($(USE_CC2420X_EXPERIMENTAL),yes)
		
		CFLAGS += -I$(SZTETOSDIR)/chips/cc2420x
		CFLAGS += -I$(SZTETOSDIR)/chips/cc2420x/util
		CFLAGS += -I$(SZTETOSDIR)/chips/cc2420x/layers
		CFLAGS += -I$(SZTETOSDIR)/platforms/telosa/chips/cc2420x

		CFLAGS += -DRADIO_CC2420X
		
	endif
endif
