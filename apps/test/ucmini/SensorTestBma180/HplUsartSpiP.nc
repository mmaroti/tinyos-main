module HplUsartSpiP {
  provides interface UsartSpi;
  provides interface McuPowerOverride;
  uses interface GeneralIO as SS;
  uses interface GeneralIO as SCK;
  uses interface McuPowerState as Mcu;
}
implementation {
  async command void UsartSpi.SpiInit(uint8_t bitRateInMhz) {
    UBRR0 = 0;
    call SCK.makeOutput();
    UCSR0C = (1<<UMSEL00) | (1<<UMSEL01);
    call UsartSpi.setClockPolarity(TRUE);
    call UsartSpi.setClockPhase(TRUE);
    call UsartSpi.enableSpi(TRUE);
    call UsartSpi.setClock(bitRateInMhz);
  }

  async command uint8_t UsartSpi.read() {
    return UDR0;
  }

  async command void UsartSpi.write(uint8_t data) {
    UDR0 = data;
  }

  async command void UsartSpi.chipSelect() {
    call SCK.makeOutput();
    call SCK.clr();
  }

  async command void UsartSpi.chipDeselect() {
    call SCK.makeOutput();
    call SCK.set();
  }

  async command void  UsartSpi.setClock(uint8_t speedInMHz) {
    UBRR0 = (16000000 / (2 * (uint32_t)speedInMHz * 1000000)) - 1;
  }

  async command void UsartSpi.setClockPolarity(bool highWhenIdle) {
    if (highWhenIdle) {
      SET_BIT(UCSR0C, UCPOL0);
    }
    else {
      CLR_BIT(UCSR0C, UCPOL0);
    }
  }

  async command bool UsartSpi.getClockPolarity() {
    return READ_BIT(UCSR0C, UCPOL0);
  }

  async command void UsartSpi.setClockPhase(bool sampleOnTrailing) {
    if (sampleOnTrailing) {
      SET_BIT(UCSR0C, UCPHA0);
    }
    else {
      CLR_BIT(UCSR0C, UCPHA0);
    }
  }

  async command bool UsartSpi.getClockPhase() {
     return READ_BIT(UCSR0C, UCPHA0);
  }

  async command void UsartSpi.enableSpi(bool enabled) {
    if (enabled) {
      UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
    }
    else {
      UCSR0B &= ~((1 << RXEN0) | (1 << TXEN0));
    }
    call Mcu.update();
  }

  async command mcu_power_t McuPowerOverride.lowestState() {
		if( (UCSR0B & (1 << RXEN0 | 1 << TXEN0)) && (UCSR0C & (1 << UMSEL01 | 1<< UMSEL00)) ) {
			return ATM128_POWER_IDLE;
		}
		else
			return ATM128_POWER_DOWN;
	}

}
