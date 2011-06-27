#ifndef PAGESTORAGE_H
#define PAGESTORAGE_H

#if defined(PLATFORM_EPIC) || defined(PLATFORM_IRIS) || defined(PLATFORM_MICA) || defined(PLATFORM_MICA2) || defined(PLATFORM_MICA2DOT) || defined(PLATFORM_MICAZ) || defined(PLATFORM_MULLE) || defined(PLATFORM_TELOSA) || defined(PLATFORM_TINYNODE)
  #define AT45DB
	#include "Storage.h"
 // #include "HplAt45db_chip.h"
  #define PAGE_SIZE AT45_PAGE_SIZE
#elif defined(PLATFORM_TELOSB) || defined(PLATFORM_UCDUAL) || defined(PLATFORM_UCMINI) || defined(PLATFORM_Z1)
  #define STM25P
  #include "Stm25p.h"
  #define PAGE_SIZE STM25P_PAGE_SIZE
#endif



#endif