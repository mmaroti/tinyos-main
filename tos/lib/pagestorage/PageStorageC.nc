#include "PageStorage.h"
generic configuration PageStorageC(volume_id_t volume_id){
  provides interface PageStorage;
}
implementation{
  #ifdef AT45DB
    components new At45dbPageStorageC(volume_id) as RealPageStorageC;
  #else if defined(STM25P)
    components new Stm25pPageStorageC(volume_id) as RealPageStorageC;
  #endif
  PageStorage=RealPageStorageC;
}