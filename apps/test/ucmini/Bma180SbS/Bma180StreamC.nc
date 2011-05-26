#include "Bma180.h" 

generic configuration Bma180StreamC() {
  provides interface ReadStream<bma180_data_t>;
}
implementation {
  components new Bma180ReadStreamClientC();
  
  ReadStream = Bma180ReadStramClientC;
}
