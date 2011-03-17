 #include <avr/eeprom.h> 
generic module EepromVariableUint16P(uint16_t address){
  provides interface Get<uint16_t>;
  provides interface Set<uint16_t>;
}
implementation {
  command uint16_t Get.get(){
    while(!eeprom_is_ready());
    
    return eeprom_read_word((uint16_t*)address);

  }
  
  command void Set.set(uint16_t value){
    while(!eeprom_is_ready());
    
    eeprom_update_word((uint16_t*)address, value);
  }
  
  
}