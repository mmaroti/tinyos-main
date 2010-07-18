#include "PacketSniffer.h"

module SerialPacketInfoPacketSnifferP {
  provides interface SerialPacketInfo as Info;
}

implementation {
  enum {
    SNIFFER_SIZE = sizeof(sniffer_data_t)
  };
  
#define MAKE_SIZE_ENUM(TYPE) \
  enum { HEADER_SIZE = sizeof(TYPE##_header_t), \
         FOOTER_SIZE = sizeof(TYPE##_footer_t), \
         MTDATA_SIZE = sizeof(TYPE##_metadata_t) };
         
#if defined(RADIO_IS_RF230)
  MAKE_SIZE_ENUM(rf230packet)
#elif defined(RADIO_IS_CC2420)
  MAKE_SIZE_ENUM(cc2420)
#elif defined(RADIO_IS_CC1000)
  MAKE_SIZE_ENUM(cc1000)
#elif defined(RADIO_IS_TDA5250)
  MAKE_SIZE_ENUM(tda5250)
#endif

  async command uint8_t Info.offset() {
    return sizeof(message_header_t)-HEADER_SIZE;
  }
  async command uint8_t Info.dataLinkLength(message_t* msg, uint8_t upperLen) {
    return upperLen + HEADER_SIZE + FOOTER_SIZE + SNIFFER_SIZE;
  }
  async command uint8_t Info.upperLength(message_t* msg, uint8_t dataLinkLen) {
    return dataLinkLen - (HEADER_SIZE + FOOTER_SIZE + SNIFFER_SIZE);
  }
}
