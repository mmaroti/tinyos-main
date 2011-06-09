
/**
 * We do this because this configuration will not get instantiated
 * more than once, so the At45dbC will be wired ot DirectStorageP
 * exactly one time. 
 * @author David Moss
 */
 
configuration At45dbWireDirectStorageC {
}

implementation {
  components At45dbDirectStorageP,
      At45dbC;
      
  At45dbDirectStorageP.At45db -> At45dbC;
  
}

