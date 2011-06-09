#if defined(PLATFORM_EPIC) || defined(PLATFORM_IRIS) || defined(PLATFORM_MICA) || defined(PLATFORM_MICA2) || defined(PLATFORM_MICA2DOT) || defined(PLATFORM_MICAZ) || defined(PLATFORM_MULLE) || defined(PLATFORM_TELOSA) || defined(PLATFORM_TINYNODE)
  #include "Storage.h"
#elif defined(PLATFORM_TELOSB) || defined(PLATFORM_UCDUAL) || defined(PLATFORM_UCMINI) || defined(PLATFORM_Z1)
  #include "Stm25p.h"
#endif
generic configuration DirectStorageC(volume_id_t volume_id) {
  provides {
    interface DirectStorage;
    interface DirectModify;
    interface VolumeSettings as DirectStorageSettings;
    interface VolumeSettings as DirectModifySettings;
  }
}
implementation{
  #if defined(PLATFORM_EPIC) || defined(PLATFORM_IRIS) || defined(PLATFORM_MICA) || defined(PLATFORM_MICA2) || defined(PLATFORM_MICA2DOT) || defined(PLATFORM_MICAZ) || defined(PLATFORM_MULLE) || defined(PLATFORM_TELOSA) || defined(PLATFORM_TINYNODE)
    components new At45dbDirectStorageC(volume_id) as RealStorage;
  #elif defined(PLATFORM_TELOSB) || defined(PLATFORM_UCDUAL) || defined(PLATFORM_UCMINI) || defined(PLATFORM_Z1)
    components new Stm25pDirectStorageC(volume_id) as RealStorage;
  #endif
  DirectStorage=RealStorage.DirectStorage;
  DirectModify=RealStorage.DirectModify;
  DirectStorageSettings=RealStorage.DirectStorageSettings;
  DirectModifySettings=RealStorage.DirectModifySettings;
}