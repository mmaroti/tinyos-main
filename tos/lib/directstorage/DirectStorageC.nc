generic configuration DirectStorageC(volume_id_t volume_id) {
  provides {
    interface DirectStorage;
    interface DirectModify;
    interface VolumeSettings as DirectStorageSettings;
    interface VolumeSettings as DirectModifySettings;
  }
}
implementation{
  #if defined(PLATFORM_EPIC) || defined(PLATFORM_IRIS) || defined(PLATFORM_MICA) || defined(PLATFORM_MICA2) || defined(PLATFORM_MICA2DOT) || defined(PLATFORM_MICAZ) || defined(PLATFORM_MULLE) || defined(PLATFORM_TELOSA || defined(PLATFORM_TINYNODE)
    components At45dbDirectStorageC as RealStorage;
  #elif defined(PLATFORM_TELOSB) || defined(PLATFORM_UCDUAL) || defined(PLATFORM_UCMINI) || defined(PLATFORM_Z1)
    components Stm25pDirectStorageC as RealStorage;
  #endif
  DirectStorage->RealStorage;
  DirectModify->RealStorage;
  DirectStorageSettings->RealStorage;
  DirectModifySettings->RealStorage;
}