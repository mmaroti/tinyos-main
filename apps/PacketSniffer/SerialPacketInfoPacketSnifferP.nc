#include "PacketSniffer.h"

module SerialPacketInfoPacketSnifferP {
  provides interface SerialPacketInfo as Info;
}

implementation {
#if defined(PLATFORM_IRIS) || defined(PLATFORM_MULLE)
  enum {
    HEADER_SIZE = sizeof(rf230packet_header_t),
    FOOTER_SIZE = sizeof(rf230packet_footer_t) + sizeof(sniffer_extra_data_t),
  };
#else
  enum {
    HEADER_SIZE = sizeof(cc2420_header_t),
    FOOTER_SIZE = sizeof(cc2420_footer_t) + sizeof(sniffer_extra_data_t),
  };
#endif

  async command uint8_t Info.offset() {
    return sizeof(message_header_t)-HEADER_SIZE;
  }
  async command uint8_t Info.dataLinkLength(message_t* msg, uint8_t upperLen) {
    return upperLen + HEADER_SIZE + FOOTER_SIZE;
  }
  async command uint8_t Info.upperLength(message_t* msg, uint8_t dataLinkLen) {
    return dataLinkLen - (HEADER_SIZE + FOOTER_SIZE);
  }
}
