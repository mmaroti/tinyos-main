#include "Stm25p.h"
generic configuration Stm25pPageStorageC(volume_id_t volume_id) {
  provides interface PageStorage;
}

implementation {
  
  
  enum {
    PAGE_ID = unique( "Stm25p.Page" ),
    VOLUME_ID = unique( "Stm25p.Volume" ),
  };

  components Stm25pPageStorageP;
  PageStorage = Stm25pPageStorageP.PageStorage[ PAGE_ID ];

  components Stm25pSectorC as SectorC;
  Stm25pPageStorageP.ClientResource[ PAGE_ID ] -> SectorC.ClientResource[ VOLUME_ID ];
  Stm25pPageStorageP.Sector[ PAGE_ID ] -> SectorC.Sector[ VOLUME_ID ];

  components new Stm25pBinderP( volume_id ) as BinderP;
  BinderP.Volume -> SectorC.Volume[ VOLUME_ID ];
}