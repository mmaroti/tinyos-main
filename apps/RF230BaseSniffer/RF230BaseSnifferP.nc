module RF230BaseSnifferP @safe() {
  uses {
    interface Boot;
    interface SplitControl as SerialControl;
    interface RadioState as RadioControl;
    interface Leds;

//  Serial communication    
    interface AMSend as UartSend[am_id_t id];
    interface Receive as UartReceive[am_id_t id];
    interface Packet as UartPacket;
    interface AMPacket as UartAMPacket;

//  Radio message detection
    interface RadioReceive;

//  Packet info
    interface RadioPacket;
  }
}

implementation
{

  message_t  uartQueueBufs[UART_QUEUE_LEN];
  message_t  * ONE_NOK uartQueue[UART_QUEUE_LEN];
  uint8_t    uartIn, uartOut;
  bool       uartBusy, uartFull;


  task void uartSendTask();

  /**  BRING UP SERIAL AND RADIO  **/
  task void serialPowerUp() {
    if ( SUCCESS != call SerialControl.start() )
      post serialPowerUp();
  }

  task void radioPowerUp() {
    if ( SUCCESS != call RadioControl.turnOn() )
      post radioPowerUp();
  }

  event void Boot.booted() {
    uint16_t i;
    for (i = 0; i < UART_QUEUE_LEN; i++)
        uartQueue[i] = &uartQueueBufs[i];
    uartIn = uartOut = 0;
    uartBusy = FALSE;
    uartFull = TRUE;

    post serialPowerUp();
  }

  event void SerialControl.startDone(error_t error) {
    if ( SUCCESS != error )
      post serialPowerUp();
    else {
      uartFull = FALSE;
      post radioPowerUp();
    }
  }

  event void SerialControl.stopDone(error_t error) {
    post serialPowerUp();
  }

  event void RadioControl.done() {
    call Leds.led0On();
  }


  /** HEADER RECEPTION **/
  /** Accept all headers to proceed to message reception **/
  event bool RadioReceive.header(message_t* msg) { return TRUE; }

  /** MESSAGE RECEPTION **/
  event message_t* ONE RadioReceive.receive(message_t* ONE msg) {

    message_t *ret = msg;
    call Leds.led1Toggle();

    atomic {
      if (!uartFull){
        ret = uartQueue[uartIn];
        uartQueue[uartIn] = msg;

        uartIn = (uartIn + 1) % UART_QUEUE_LEN;
	
        if (uartIn == uartOut)
    	    uartFull = TRUE;

    	  if (!uartBusy) {
  	      post uartSendTask();
	        uartBusy = TRUE;
	      }
	    }
    }
    
    return ret;
  }

  /** SERIAL COMMUNICATION **/
  task void uartSendTask() {

    uint8_t len;
    //am_id_t id;
    //am_addr_t addr, src;
    message_t* msg;

    atomic {
      if (uartIn == uartOut && !uartFull) {
    	  uartBusy = FALSE;
	      return;
    	}
    }

    msg = uartQueue[uartOut];
    len = call RadioPacket.payloadLength(msg);
    //id = call RadioAMPacket.type(msg);
    //addr = call RadioAMPacket.destination(msg);
    //src = call RadioAMPacket.source(msg);
    call UartPacket.clear(msg);
    call UartAMPacket.setSource(msg, 0xaaaa);

    if (call UartSend.send[0](0xebeb, uartQueue[uartOut], len) != SUCCESS)
	    post uartSendTask();

  }

  event void UartSend.sendDone[am_id_t id](message_t* msg, error_t error) {
    if ( error == SUCCESS )
      call Leds.led2Toggle();
      atomic {
      	if (msg == uartQueue[uartOut]) {
  	      if (++uartOut >= UART_QUEUE_LEN)
	          uartOut = 0;
	        if (uartFull)
	          uartFull = FALSE;
	      }
      }
      post uartSendTask();
  }

  event message_t* UartReceive.receive[am_id_t id](message_t* msg, void* payload, uint8_t len) {
    return msg;
  }

}  
