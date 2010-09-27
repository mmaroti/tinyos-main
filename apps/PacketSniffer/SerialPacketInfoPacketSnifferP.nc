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
         
#if defined(RADIO_RF230)
  MAKE_SIZE_ENUM(rf230packet)
#elif defined(RADIO_CC2420)
  MAKE_SIZE_ENUM(cc2420)
#elif defined(RADIO_CC1000)
  MAKE_SIZE_ENUM(cc1000)
#elif defined(RADIO_TDA5250)
  MAKE_SIZE_ENUM(tda5250)
#elif defined(RADIO_XE1205)
  MAKE_SIZE_ENUM(xe1205)
#endif


  async command uint8_t Info.offset() {
    return 0;
  }
  async command uint8_t Info.dataLinkLength(message_t* msg, uint8_t upperLen) {
    return upperLen + FOOTER_SIZE + SNIFFER_SIZE + sizeof(uart_id_t);
  }
  async command uint8_t Info.upperLength(message_t* msg, uint8_t dataLinkLen) {
    return dataLinkLen - (FOOTER_SIZE + SNIFFER_SIZE + sizeof(uart_id_t));
  }
}
