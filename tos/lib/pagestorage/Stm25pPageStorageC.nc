#include "Stm25p.h"
generic configuration Stm25pPageStorageC(volume_id_t volume_id) {
  provides interface PageStorage;
}

implementation {

  enum{
    PAGE_ID=unique("Stm25p.Page"),
  };
  components Stm25pPageStorageP, Stm25pSpiC, MainC;
  MainC.SoftwareInit -> Stm25pSpiC;
  
  Stm25pPageStorageP.Stm25pSpi->Stm25pSpiC;
  Stm25pPageStorageP.Resource->Stm25pSpiC;
  
  PageStorage=Stm25pPageStorageP.PageStorage[PAGE_ID];
  
  components new Stm25pBinderP( volume_id ) as BinderP;
  BinderP.Volume -> Stm25pPageStorageP.Volume[PAGE_ID];
}
