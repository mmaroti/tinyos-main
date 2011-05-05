interface UsartSpi {
  async command void SpiInit(uint8_t bitRateInMhz);

  async command uint8_t read();

  async command void write(uint8_t data);

  async command void chipSelect();

  async command void chipDeselect();

  async command void  setClock(uint8_t speed);

  async command void setClockPolarity(bool highWhenIdle);

  async command bool getClockPolarity();

  async command void setClockPhase(bool sampleOnTrailing);

  async command bool getClockPhase();

  async command void enableSpi(bool enabled);
}
