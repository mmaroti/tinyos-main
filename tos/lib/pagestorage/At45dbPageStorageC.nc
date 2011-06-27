#include "Storage.h"
#include "At45db.h"
generic configuration At45dbPageStorageC(volume_id_t volume_id) {
  provides interface PageStorage;
}

implementation {

  enum {
    PAGE_ID = unique("at45db.page"),
    RESOURCE_ID = unique(UQ_AT45DB),
  };
    
  components At45dbPageStorageP, At45dbStorageManagerC, At45dbC;

  PageStorage = At45dbPageStorageP.PageStorage[PAGE_ID];

  At45dbPageStorageP.At45dbVolume[PAGE_ID] -> At45dbStorageManagerC.At45dbVolume[volume_id];  
  At45dbPageStorageP.Resource[PAGE_ID] -> At45dbC.Resource[PAGE_ID];
  At45dbPageStorageP.At45db->At45dbC;
  
}